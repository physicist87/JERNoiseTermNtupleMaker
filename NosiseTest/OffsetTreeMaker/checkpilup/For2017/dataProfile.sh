#!/bin/sh

#pileupCalc.py -i ../Cert_314472-325175_13TeV_Legacy2018_Collisions18_JSON.txt --inputLumiJSON pileup_latest.txt --calcMode true --minBiasXsec 69200 --maxPileupBin 100 --numPileupBins 100 MyDataPileupHistogram.root

PILEUP_LATEST=./pileup_latest.txt
JSON=./Cert_294927-306462_13TeV_UL2017_Collisions17_GoldenJSON.txt
Study=For_2017

#if [ ! -f "$PILEUP_LATEST" ]; then
#   echo "File $PILEUP_LATEST does not exist on this site, copying from lxplus"
#   scp $USER@lxplus.cern.ch:$PILEUP_LATEST pileup_latest.txt
#   PILEUP_LATEST=pileup_latest.txt
#fi

echo $JSON
pileupCalc.py -i $JSON --inputLumiJSON $PILEUP_LATEST --calcMode true --minBiasXsec 69200 --maxPileupBin 100 --numPileupBins 100 ${Study}_XSecCentral.root
echo $JSON
pileupCalc.py -i $JSON --inputLumiJSON $PILEUP_LATEST --calcMode true --minBiasXsec 72660 --maxPileupBin 100 --numPileupBins 100 ${Study}_XSecCentral5pUp.root
echo $JSON
pileupCalc.py -i $JSON --inputLumiJSON $PILEUP_LATEST --calcMode true --minBiasXsec 76120 --maxPileupBin 100 --numPileupBins 100 ${Study}_XSecCentral10pUp.root
echo $JSON
pileupCalc.py -i $JSON --inputLumiJSON $PILEUP_LATEST --calcMode true --minBiasXsec 62280 --maxPileupBin 100 --numPileupBins 100 ${Study}_XSec10pDown.root
echo $JSON
pileupCalc.py -i $JSON --inputLumiJSON $PILEUP_LATEST --calcMode true --minBiasXsec 65740 --maxPileupBin 100 --numPileupBins 100 ${Study}_XSec5pDown.root

