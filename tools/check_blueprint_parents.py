import unreal

BLUEPRINTS = [
    "/Game/Blueprints/BP_Ball",
    "/Game/Blueprints/BP_CueBall",
    "/Game/Blueprints/BP_Cue",
    "/Game/Blueprints/BP_AimingAid",
    "/Game/Blueprints/BP_GameMode_8Ball",
]


for bp_path in BLUEPRINTS:
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_warning(f"Missing blueprint: {bp_path}")
        continue

    generated_class = bp.generated_class
    if not generated_class:
        unreal.log_warning(f"{bp_path} has no generated_class")
        continue

    parent_class = generated_class.get_super_class()
    if parent_class:
        unreal.log(f"{bp_path} parent_class: {parent_class.get_name()}")
    else:
        unreal.log_warning(f"{bp_path} has no parent_class")
