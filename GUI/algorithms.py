EXACT_ALGO=["AllSplits"]

SEMI_EXACT_ALGO=["AllEdgeByEdge", "ConvexSplits"]

HEURISTICS=["GreedyEdgeSort", "OneSideDimbyDim"]

NULL=0
OPTIONAL=1
MANDATORY=2

"""main_alg, dmin, dmax, sub_alg, start_sol, time, test"""
NONE=dict({"main_alg":MANDATORY, "dmin":NULL, "dmax":NULL, "sub_alg":NULL, "start_sol":NULL, "time":NULL, "test":NULL})

ALLSPLITS=dict({"main_alg":MANDATORY, "dmin":NULL, "dmax":NULL, "sub_alg":NULL, "start_sol":OPTIONAL, "time":OPTIONAL, "test":NULL})

ALLEDGEBYEDGE=dict({"main_alg":MANDATORY, "dmin":NULL, "dmax":NULL, "sub_alg":NULL, "start_sol":OPTIONAL, "time":OPTIONAL, "test":OPTIONAL})

CONVEXSPLITS=dict({"main_alg":MANDATORY, "dmin":NULL, "dmax":OPTIONAL, "sub_alg":NULL, "start_sol":OPTIONAL, "time":OPTIONAL, "test":NULL})

GREEDYEDGESORT=dict({"main_alg":MANDATORY, "dmin":NULL, "dmax":NULL, "sub_alg":NULL, "start_sol":OPTIONAL, "time":OPTIONAL, "test":OPTIONAL})

SHUFFLE=dict({"main_alg":MANDATORY, "dmin":OPTIONAL, "dmax":OPTIONAL, "sub_alg":NULL, "start_sol":MANDATORY, "time":OPTIONAL, "test":NULL})

ONESIDEDIMBYDIM=dict({"main_alg":MANDATORY, "dmin":NULL, "dmax":OPTIONAL, "sub_alg":NULL, "start_sol":OPTIONAL, "time":OPTIONAL, "test":OPTIONAL})

SPLITSDIMBYDIM=dict({"main_alg":MANDATORY, "dmin":OPTIONAL, "dmax":OPTIONAL, "sub_alg":NULL, "start_sol":OPTIONAL, "time":OPTIONAL, "test":NULL})

ALGORITHMS=dict({"None":NONE, "AllSplits":ALLSPLITS, "AllEdgeByEdge":ALLEDGEBYEDGE, "ConvexSplits":CONVEXSPLITS, "GreedyEdgeSort":GREEDYEDGESORT, "OneSideDimByDim":ONESIDEDIMBYDIM, "SplitsDimByDim":SPLITSDIMBYDIM})
