import unreal


BP_PATH = "/Game/Blueprints/BP_Table"
PARENT_CLASS_PATH = "/Script/billiardgame.BilliardTableActor"


bp = unreal.load_asset(BP_PATH)
if not bp:
    raise RuntimeError(f"Failed to load blueprint: {BP_PATH}")

parent_class = unreal.load_class(None, PARENT_CLASS_PATH)
if not parent_class:
    raise RuntimeError(f"Failed to load parent class: {PARENT_CLASS_PATH}")

unreal.BlueprintEditorLibrary.reparent_blueprint(bp, parent_class)
unreal.BlueprintEditorLibrary.compile_blueprint(bp)
unreal.EditorAssetLibrary.save_loaded_asset(bp)
unreal.log(f"Reparented {BP_PATH} to {PARENT_CLASS_PATH}")
