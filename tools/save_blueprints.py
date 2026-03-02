import unreal


def save_asset(path: str) -> None:
    if unreal.EditorAssetLibrary.does_asset_exist(path):
        unreal.EditorAssetLibrary.save_asset(path, only_if_is_dirty=False)
        unreal.log(f"Saved asset: {path}")
    else:
        unreal.log_warning(f"Missing asset: {path}")


ASSETS = [
    "/Game/Blueprints/BP_Ball",
    "/Game/Blueprints/BP_Table",
    "/Game/Blueprints/BP_CueBall",
    "/Game/Blueprints/BP_Cue",
    "/Game/Blueprints/BP_PocketTrigger",
    "/Game/Blueprints/BP_GameMode_8Ball",
    "/Game/Blueprints/BP_GameState_8Ball",
    "/Game/Blueprints/BP_PlayerController",
]


for asset_path in ASSETS:
    save_asset(asset_path)

