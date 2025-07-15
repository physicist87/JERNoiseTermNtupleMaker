#!/bin/tcsh

#################
###Input files###
#################
set inputlists = ("TopptReweightVsNon" "TopptReweightVsNon_others")
#set inputlists = ("TopptReweightVsNon" )
set channels = ("MuMu" "MuEl" "ElEl")
#set channels = ("MuMu" )
foreach i ( $inputlists )
    mkdir -p Output
    #./Simple_Overlay ${i}.list Output/${i} MyConf.cfg 
    ./Simple_Overlay ${i}.list Output/${i} PileUp.cfg 
end
