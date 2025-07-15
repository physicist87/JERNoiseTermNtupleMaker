#!/bin/sh

#pileupCalc.py -i ../Cert_314472-325175_13TeV_Legacy2018_Collisions18_JSON.txt --inputLumiJSON pileup_latest.txt --calcMode true --minBiasXsec 69200 --maxPileupBin 100 --numPileupBins 100 MyDataPileupHistogram.root

PILEUP_LATEST=./pileup_latest.txt
PILEUP_PostVFP=./pileup_latest_postVFP.txt
PILEUP_PreVFP=./pileup_latest_preVFP.txt
JSON=./Cert_271036-284044_13TeV_Legacy2016_Collisions16_JSON.txt
Study=For_2016

#if [ ! -f "$PILEUP_LATEST" ]; then
#   echo "File $PILEUP_LATEST does not exist on this site, copying from lxplus"
#   scp $USER@lxplus.cern.ch:$PILEUP_LATEST pileup_latest.txt
#   PILEUP_LATEST=pileup_latest.txt
#fi

echo $JSON $PILEUP_LATEST
pileupCalc.py -i $JSON --inputLumiJSON $PILEUP_LATEST --calcMode true --minBiasXsec 69200 --maxPileupBin 100 --numPileupBins 100 ${Study}_Total.root

echo $JSON $PILEUP_PreVFP
pileupCalc.py -i $JSON --inputLumiJSON $PILEUP_PreVFP --calcMode true --minBiasXsec 69200 --maxPileupBin 100 --numPileupBins 100 ${Study}_PreVFP.root

echo $JSON $PILEUP_PostVFP
pileupCalc.py -i $JSON --inputLumiJSON $PILEUP_PostVFP --calcMode true --minBiasXsec 69200 --maxPileupBin 100 --numPileupBins 100 ${Study}_PostVFP.root
