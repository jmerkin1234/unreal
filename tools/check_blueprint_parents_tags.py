import unreal

BLUEPRINTS = [
    "/Game/Blueprints/BP_Ball",
    "/Game/Blueprints/BP_CueBall",
    "/Game/Blueprints/BP_Cue",
    "/Game/Blueprints/BP_AimingAid",
    "/Game/Blueprints/BP_GameMode_8Ball",
    "/Game/Blueprints/BP_Table",
]

registry = unreal.AssetRegistryHelpers.get_asset_registry()

for bp_path in BLUEPRINTS:
    asset_data = registry.get_asset_by_object_path(f"{bp_path}.{bp_path.split('/')[-1]}")
    if not asset_data.is_valid():
        unreal.log_warning(f"Missing asset data: {bp_path}")
        continue

    parent_class = asset_data.get_tag_value("ParentClass")
    unreal.log(f"{bp_path} ParentClass tag: {parent_class}")
