#!/bin/sh

#pileupCalc.py -i ../Cert_314472-325175_13TeV_Legacy2018_Collisions18_JSON.txt --inputLumiJSON pileup_latest.txt --calcMode true --minBiasXsec 69200 --maxPileupBin 100 --numPileupBins 100 MyDataPileupHistogram.root

PILEUP_LATEST=./pileup_latest.txt
Study=v1

#if [ ! -f "$PILEUP_LATEST" ]; then
#   echo "File $PILEUP_LATEST does not exist on this site, copying from lxplus"
#   scp $USER@lxplus.cern.ch:$PILEUP_LATEST pileup_latest.txt
#   PILEUP_LATEST=pileup_latest.txt
#fi

pileupCalc.py -i ./Cert_314472-325175_13TeV_Legacy2018_Collisions18_JSON_RunA.txt --inputLumiJSON $PILEUP_LATEST --calcMode true --minBiasXsec 69200 --maxPileupBin 100 --numPileupBins 100 ${Study}_RunA.root
pileupCalc.py -i ./Cert_314472-325175_13TeV_Legacy2018_Collisions18_JSON_RunB.txt --inputLumiJSON $PILEUP_LATEST --calcMode true --minBiasXsec 69200 --maxPileupBin 100 --numPileupBins 100 ${Study}_RunB.root
pileupCalc.py -i ./Cert_314472-325175_13TeV_Legacy2018_Collisions18_JSON_RunC.txt --inputLumiJSON $PILEUP_LATEST --calcMode true --minBiasXsec 69200 --maxPileupBin 100 --numPileupBins 100 ${Study}_RunC.root
pileupCalc.py -i ./Cert_314472-325175_13TeV_Legacy2018_Collisions18_JSON_RunD.txt --inputLumiJSON $PILEUP_LATEST --calcMode true --minBiasXsec 69200 --maxPileupBin 100 --numPileupBins 100 ${Study}_RunD.root
