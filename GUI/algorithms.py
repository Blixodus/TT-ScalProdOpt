EXACT_ALGO=["AllSplits"]

SEMI_EXACT_ALGO=["AllEdgeByEdge", "ConvexSplits"]

HEURISTICS=["GreedyEdgeSort", "1SideDimbyDim"]

NULL=0
OPTIONAL=1
MANDATORY=2

"""main_alg, dmin, dmax, sub_alg, start_sol, time, test"""
NONE=dict({"main_alg":MANDATORY, "dmin":NULL, "dmax":NULL, "sub_alg":NULL, "start_sol":NULL, "time":NULL, "test":NULL})

ALLSPLITS=dict({"main_alg":MANDATORY, "dmin":NULL, "dmax":NULL, "sub_alg":NULL, "start_sol":NULL, "time":OPTIONAL, "test":NULL})

ALLEDGEBYEDGE=dict({"main_alg":MANDATORY, "dmin":NULL, "dmax":NULL, "sub_alg":NULL, "start_sol":NULL, "time":OPTIONAL, "test":NULL})

CONVEXSPLITS=dict({"main_alg":MANDATORY, "dmin":NULL, "dmax":OPTIONAL, "sub_alg":NULL, "start_sol":NULL, "time":OPTIONAL, "test":NULL})

GREEDYEDGESORT=dict({"main_alg":MANDATORY, "dmin":NULL, "dmax":NULL, "sub_alg":NULL, "start_sol":NULL, "time":OPTIONAL, "test":NULL})

SHUFFLE=dict({"main_alg":MANDATORY, "dmin":OPTIONAL, "dmax":OPTIONAL, "sub_alg":NULL, "start_sol":OPTIONAL, "time":OPTIONAL, "test":NULL})

ONESIDEDIMBYDIM=dict({"main_alg":MANDATORY, "dmin":NULL, "dmax":OPTIONAL, "sub_alg":NULL, "start_sol":NULL, "time":OPTIONAL, "test":NULL})

SPLITSDIMBYDIM=dict({"main_alg":MANDATORY, "dmin":OPTIONAL, "dmax":OPTIONAL, "sub_alg":NULL, "start_sol":NULL, "time":OPTIONAL, "test":NULL})

ALGORITHMS=dict({"None":NONE, "AllSplits":ALLSPLITS, "AllEdgeByEdge":ALLEDGEBYEDGE, "ConvexSplits":CONVEXSPLITS, "GreedyEdgeSort":GREEDYEDGESORT, "OneSideDimByDim":ONESIDEDIMBYDIM, "SplitsDimByDim":SPLITSDIMBYDIM})
