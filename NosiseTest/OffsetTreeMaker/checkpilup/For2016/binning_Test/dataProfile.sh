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

#echo $JSON $PILEUP_LATEST "Bin 100"
#pileupCalc.py -i $JSON --inputLumiJSON $PILEUP_LATEST --calcMode true --minBiasXsec 69200 --maxPileupBin 100 --numPileupBins 100 ${Study}_Bin100.root

#echo $JSON $PILEUP_LATEST "Bin 99"
#pileupCalc.py -i $JSON --inputLumiJSON $PILEUP_LATEST --calcMode true --minBiasXsec 69200 --maxPileupBin 99 --numPileupBins 99 ${Study}_Bin99.root

#echo $JSON $PILEUP_PreVFP "Bin 100"
#pileupCalc.py -i $JSON --inputLumiJSON $PILEUP_PreVFP --calcMode true --minBiasXsec 69200 --maxPileupBin 100 --numPileupBins 100 ${Study}_PreVFP_Bin100.root

#echo $JSON $PILEUP_PreVFP "Bin 99"
#pileupCalc.py -i $JSON --inputLumiJSON $PILEUP_PreVFP --calcMode true --minBiasXsec 69200 --maxPileupBin 99 --numPileupBins 99 ${Study}_PreVFP_Bin99.root

#echo $JSON $PILEUP_PostVFP "Bin 100"
#pileupCalc.py -i $JSON --inputLumiJSON $PILEUP_PostVFP --calcMode true --minBiasXsec 69200 --maxPileupBin 100 --numPileupBins 100 ${Study}_PostVFP_Bin100.root

#echo $JSON $PILEUP_PostVFP "Bin 99"
#pileupCalc.py -i $JSON --inputLumiJSON $PILEUP_PostVFP --calcMode true --minBiasXsec 69200 --maxPileupBin 99 --numPileupBins 99 ${Study}_PostVFP_Bin99.root

#echo $JSON $PILEUP_LATEST "Bin 200"
#pileupCalc.py -i $JSON --inputLumiJSON $PILEUP_LATEST --calcMode true --minBiasXsec 69200 --maxPileupBin 200 --numPileupBins 200 ${Study}_Bin200.root

#echo $JSON $PILEUP_PreVFP "Bin 1000"
#pileupCalc.py -i $JSON --inputLumiJSON $PILEUP_PreVFP --calcMode true --minBiasXsec 692000 --maxPileupBin 1000 --numPileupBins 1000 ${Study}_PreVFP_Bin1000.root

#echo $JSON $PILEUP_PostVFP "Bin 1000"
#pileupCalc.py -i $JSON --inputLumiJSON $PILEUP_PostVFP --calcMode true --minBiasXsec 69200 --maxPileupBin 1000 --numPileupBins 1000 ${Study}_PostVFP_Bin1000.root

echo $JSON $PILEUP_LATEST "Bin 1000"
pileupCalc.py -i $JSON --inputLumiJSON $PILEUP_LATEST --calcMode true --minBiasXsec 69200 --maxPileupBin 1000 --numPileupBins 1000 ${Study}_Bin1000.root
