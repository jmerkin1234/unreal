import unreal


def create_blueprint_if_missing(asset_name: str, parent_class) -> None:
    asset_path = f"/Game/Blueprints/{asset_name}"
    if unreal.EditorAssetLibrary.does_asset_exist(asset_path):
        unreal.log(f"Exists: {asset_path}")
        return

    factory = unreal.BlueprintFactory()
    factory.set_editor_property("parent_class", parent_class)
    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    blueprint = asset_tools.create_asset(
        asset_name=asset_name,
        package_path="/Game/Blueprints",
        asset_class=unreal.Blueprint,
        factory=factory,
    )
    if blueprint:
        unreal.EditorAssetLibrary.save_asset(asset_path, only_if_is_dirty=False)
        unreal.log(f"Created: {asset_path}")
    else:
        unreal.log_warning(f"Failed to create: {asset_path}")


create_blueprint_if_missing("BP_PocketTrigger", unreal.Actor)
create_blueprint_if_missing("BP_GameState_8Ball", unreal.GameStateBase)
create_blueprint_if_missing("BP_PlayerController", unreal.PlayerController)

