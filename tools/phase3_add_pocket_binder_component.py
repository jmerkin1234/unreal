import unreal


BP_TABLE_PATH = "/Game/Blueprints/BP_Table"
COMPONENT_NAME = "PocketBinder"
COMPONENT_CLASS_PATH = "/Script/billiardgame.BilliardPocketBinderComponent"


bp = unreal.load_asset(BP_TABLE_PATH)
if not bp:
    raise RuntimeError(f"Failed to load: {BP_TABLE_PATH}")

component_class = unreal.load_class(None, COMPONENT_CLASS_PATH)
if not component_class:
    raise RuntimeError(f"Failed to load component class: {COMPONENT_CLASS_PATH}")

sds = unreal.get_engine_subsystem(unreal.SubobjectDataSubsystem)
sdbl = unreal.SubobjectDataBlueprintFunctionLibrary

handles = sds.k2_gather_subobject_data_for_blueprint(bp)
root_handle = None
existing_names = set()

for handle in handles:
    data = sdbl.get_data(handle)
    if sdbl.is_default_scene_root(data):
        root_handle = handle
    obj = sdbl.get_object_for_blueprint(data, bp)
    if obj:
        existing_names.add(obj.get_name())

if COMPONENT_NAME in existing_names:
    unreal.log(f"Component already exists: {COMPONENT_NAME}")
else:
    params = unreal.AddNewSubobjectParams()
    params.set_editor_property("parent_handle", root_handle)
    params.set_editor_property("new_class", component_class)
    params.set_editor_property("blueprint_context", bp)

    new_handle, fail_reason = sds.add_new_subobject(params)
    fail_str = str(fail_reason)
    if fail_str:
        raise RuntimeError(f"Failed to add component: {fail_str}")

    new_data = sdbl.get_data(new_handle)
    component = sdbl.get_object_for_blueprint(new_data, bp)
    if component:
        component.rename(COMPONENT_NAME)

    unreal.log(f"Added component: {COMPONENT_NAME}")

unreal.BlueprintEditorLibrary.compile_blueprint(bp)
unreal.EditorAssetLibrary.save_loaded_asset(bp)
