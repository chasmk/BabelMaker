// Copyright 2021 Mickael Daniel. All Rights Reserved.

#pragma once

#include "AssetToolsModule.h"
#include "Core/Logging/GASCompanionEditorLog.h"
#include "Engine/Blueprint.h"
#include "Factories/BlueprintFactory.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Templates/GSCTemplate_GameplayEffectDefinition.h"
#include "GSCCreationMenu.generated.h"

struct FGSCMenuItem : TSharedFromThis<FGSCMenuItem>
{
	TMap<FString, TSharedPtr<FGSCMenuItem>> SubItems;

	FString AssetPrefix;
	FString DefaultAssetName;
	FText TooltipText;
	TSubclassOf<UObject> ParentClass;

	static void CreateBlueprintAction(const TSharedPtr<FGSCMenuItem> InItem, const TArray<FString> InSelectedPaths)
	{
		if (InSelectedPaths.IsEmpty())
		{
			return;
		}

		const FAssetToolsModule& AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools");

		UBlueprintFactory* BlueprintFactory = NewObject<UBlueprintFactory>();

		// Init with w/e the item parent class is (can be either an UGameplayAbility or UGSCTemplate_GameplayEffectDefinition)
		UClass* EffectParentClass = InItem->ParentClass;

		// If we're dealing with Template GameplayEffectDefinition, make sure Parent class is set to UGameplayEffect
		if (InItem->ParentClass->IsChildOf(UGSCTemplate_GameplayEffectDefinition::StaticClass()))
		{
			EffectParentClass = UGameplayEffect::StaticClass();
		}

		BlueprintFactory->ParentClass = EffectParentClass;

		FString PackageName = InSelectedPaths[0];
		FString AssetName = InItem->AssetPrefix + InItem->DefaultAssetName;

		const FString DefaultFullPath = PackageName / AssetName;

		AssetToolsModule.Get().CreateUniqueAssetName(DefaultFullPath, TEXT(""), /*out*/ PackageName, /*out*/ AssetName);
		UBlueprint* Asset = Cast<UBlueprint>(AssetToolsModule.Get().CreateAsset(*AssetName, InSelectedPaths[0], UBlueprint::StaticClass(), BlueprintFactory));
		EDITOR_LOG(Verbose, TEXT("Create Asset %s with path: %s (name: %s)"), *GetNameSafe(Asset), *InSelectedPaths[0], *AssetName)

		if (Asset)
		{
			UGameplayEffect* CDO = Cast<UGameplayEffect>(Asset->GeneratedClass.GetDefaultObject());
			UGSCTemplate_GameplayEffectDefinition* TemplateCDO = Cast<UGSCTemplate_GameplayEffectDefinition>(InItem->ParentClass.GetDefaultObject());

			if (CDO && TemplateCDO)
			{
				EDITOR_LOG(Verbose, TEXT("Update Gameplay Effect CDO to match template for %s"), *GetNameSafe(CDO))
				UGSCTemplate_GameplayEffectDefinition::CopyProperties(CDO, TemplateCDO);
				Asset->Modify();
			}
		}
	}

	static void BuildMenus_r(const TSharedPtr<FGSCMenuItem> InItem, FMenuBuilder& MenuBuilder, const TArray<FString>& SelectedPaths)
	{
		for (const TPair<FString, TSharedPtr<FGSCMenuItem>>& Item : InItem->SubItems)
		{
			TSharedPtr<FGSCMenuItem> SubItem  = Item.Value;
			FString CatName = Item.Key;

			// Add a submenu if this guy has sub items
			if (SubItem->SubItems.Num() > 0)
			{
				MenuBuilder.AddSubMenu(
					FText::FromString(CatName),
					FText::FromString(CatName),
					FNewMenuDelegate::CreateLambda([SubItem, SelectedPaths](FMenuBuilder& SubMenuBuilder)
					{
						BuildMenus_r(SubItem, SubMenuBuilder, SelectedPaths);
					})
				);
			}

			// Add the actual menu item to create the new GE
			if (SubItem->ParentClass.Get() != nullptr)
			{
				MenuBuilder.AddMenuEntry(
					// note: the last category string is what we use for this. Using the "Default Asset Name" is not desirable here. (e.g., Damage|Ability|Instant but "Damage" is default asset name)
					FText::FromString(CatName),
					SubItem->TooltipText,
					FSlateIcon(),
					FUIAction(FExecuteAction::CreateStatic(&CreateBlueprintAction, SubItem, SelectedPaths))
				);
			}
		}
	}
};

UCLASS()
class GASCOMPANIONEDITOR_API UGSCCreationMenu : public UObject
{
	GENERATED_BODY()

public:

	template <typename TCreationData>
	static void TopMenuBuild(FMenuBuilder& TopMenuBuilder, const FText InMenuLabel, const FText InMenuTooltip, const TArray<FString> SelectedPaths, TArray<TCreationData> Definitions)
	{
		if (Definitions.Num() == 0)
		{
			return;
		}

		TopMenuBuilder.AddSubMenu(
			InMenuLabel,
			InMenuTooltip,
			FNewMenuDelegate::CreateLambda([SelectedPaths, Definitions](FMenuBuilder& GETopMenuBuilder)
			{
				// Loop through our Definitions and build FGSCMenuItem
				const TSharedPtr<FGSCMenuItem> RootItem = MakeShareable(new FGSCMenuItem);
				for (const TCreationData& Def : Definitions)
				{
					if (Def.ParentClass.Get() == nullptr)
					{
						continue;
					}

					TArray<FString> CategoryStrings;
					Def.MenuPath.ParseIntoArray(CategoryStrings, TEXT("|"), true);

					FGSCMenuItem* CurrentItem = RootItem.Get();
					for (int32 idx=0; idx < CategoryStrings.Num(); ++idx)
					{
						FString& Str = CategoryStrings[idx];
						TSharedPtr<FGSCMenuItem>& DestItem = CurrentItem->SubItems.FindOrAdd(Str);
						if (!DestItem.IsValid())
						{
							DestItem = MakeShareable(new FGSCMenuItem);
						}
						CurrentItem = DestItem.Get();
					}

					CurrentItem->AssetPrefix = Def.AssetPrefix;
					CurrentItem->DefaultAssetName = Def.BaseName;
					CurrentItem->TooltipText = Def.TooltipText;
					CurrentItem->ParentClass = Def.ParentClass.Get();
				}

				// Build menu delegates based on our data
				FGSCMenuItem::BuildMenus_r(RootItem, GETopMenuBuilder, SelectedPaths);
			})
		);
	}
};
