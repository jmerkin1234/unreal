import unreal

CLASS_PATHS = [
    "/Script/billiardgame.BilliardBallActor",
    "/Script/billiardgame.BilliardCueBallActor",
    "/Script/billiardgame.BilliardCueActor",
    "/Script/billiardgame.BilliardAimingAidActor",
    "/Script/billiardgame.BilliardGameMode8Ball",
]


for class_path in CLASS_PATHS:
    loaded = unreal.load_class(None, class_path)
    if loaded:
        unreal.log(f"Loaded class: {class_path}")
    else:
        unreal.log_warning(f"Missing class: {class_path}")
