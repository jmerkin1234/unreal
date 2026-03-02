import unreal


ASSET_PATH = "/Game/Blueprints"

BLUEPRINT_SPECS = [
    ("BP_Ball", "/Script/billiardgame.BilliardBallActor"),
    ("BP_CueBall", "/Script/billiardgame.BilliardCueBallActor"),
    ("BP_Cue", "/Script/billiardgame.BilliardCueActor"),
    ("BP_AimingAid", "/Script/billiardgame.BilliardAimingAidActor"),
    ("BP_GameMode_8Ball", "/Script/billiardgame.BilliardGameMode8Ball"),
]


def ensure_folder(path: str) -> None:
    if not unreal.EditorAssetLibrary.does_directory_exist(path):
        unreal.EditorAssetLibrary.make_directory(path)


def delete_asset(asset_path: str) -> None:
    if unreal.EditorAssetLibrary.does_asset_exist(asset_path):
        unreal.EditorAssetLibrary.delete_asset(asset_path)
        unreal.log(f"Deleted: {asset_path}")


def create_blueprint(asset_name: str, parent_class_path: str) -> None:
    parent_class = unreal.load_class(None, parent_class_path)
    if not parent_class:
        raise RuntimeError(f"Failed to load parent class: {parent_class_path}")

    factory = unreal.BlueprintFactory()
    factory.set_editor_property("ParentClass", parent_class)

    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    blueprint = asset_tools.create_asset(asset_name, ASSET_PATH, None, factory)
    if not blueprint:
        raise RuntimeError(f"Failed to create {asset_name}")

    unreal.EditorAssetLibrary.save_loaded_asset(blueprint)
    unreal.log(f"Created: {ASSET_PATH}/{asset_name}")


def main() -> None:
    ensure_folder(ASSET_PATH)

    for asset_name, _ in BLUEPRINT_SPECS:
        delete_asset(f"{ASSET_PATH}/{asset_name}")

    for asset_name, parent_path in BLUEPRINT_SPECS:
        create_blueprint(asset_name, parent_path)


if __name__ == "__main__":
    main()
