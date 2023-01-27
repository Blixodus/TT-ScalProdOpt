EXACT_ALGO=["AllSplits"]

SEMI_EXACT_ALGO=["EdgeByEdge", "ConvexSplits"]

HEURISTICS=["GreedyEdgeSort", "1SideDimbyDim"]

NULL=0
OPTIONAL=1
MANDATORY=2

"""main_alg, dmin, dmax, sub_alg, start_sol, time, test"""
NONE=dict({"main_alg":MANDATORY, "dmin":NULL, "dmax":NULL, "sub_alg":NULL, "start_sol":NULL, "time":NULL, "test":NULL})

ALLSPLITS=dict({"main_alg":MANDATORY, "dmin":OPTIONAL, "dmax":OPTIONAL, "sub_alg":OPTIONAL, "start_sol":OPTIONAL, "time":OPTIONAL, "test":OPTIONAL})

EDGEBYEDGE=dict({"main_alg":MANDATORY, "dmin":NULL, "dmax":NULL, "sub_alg":NULL, "start_sol":OPTIONAL, "time":OPTIONAL, "test":OPTIONAL})

CONVEXSPLITS=dict({"main_alg":MANDATORY, "dmin":OPTIONAL, "dmax":OPTIONAL, "sub_alg":OPTIONAL, "start_sol":OPTIONAL, "time":OPTIONAL, "test":OPTIONAL})

GREEDYEDGESORT=dict({"main_alg":MANDATORY, "dmin":OPTIONAL, "dmax":NULL, "sub_alg":OPTIONAL, "start_sol":NULL, "time":OPTIONAL, "test":OPTIONAL})

SHUFFLE=dict({"main_alg":MANDATORY, "dmin":OPTIONAL, "dmax":OPTIONAL, "sub_alg":NULL, "start_sol":MANDATORY, "time":OPTIONAL, "test":OPTIONAL})

ONESIDEDIMBYDIM=dict({"main_alg":MANDATORY, "dmin":NULL, "dmax":OPTIONAL, "sub_alg":NULL, "start_sol":OPTIONAL, "time":OPTIONAL, "test":OPTIONAL})

SPLITSDIMBYDIM=dict({"main_alg":MANDATORY, "dmin":OPTIONAL, "dmax":OPTIONAL, "sub_alg":OPTIONAL, "start_sol":OPTIONAL, "time":OPTIONAL, "test":OPTIONAL})

ALGORITHMS=dict({"None":NONE, "AllSplits":ALLSPLITS, "EdgeByEdge":EDGEBYEDGE, "ConvexSplits":CONVEXSPLITS, "GreedyEdgeSort":GREEDYEDGESORT, "Shuffle":SHUFFLE, "OneSideDimByDim":ONESIDEDIMBYDIM, "SplitsDimByDim":SPLITSDIMBYDIM})