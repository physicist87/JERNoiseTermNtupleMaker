#!/bin/sh

PILEUP_LATEST=/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/PileUp/pileup_latest.txt
JSON=Cert_271036-284044_13TeV_23Sep2016ReReco_Collisions16_JSON.txt
JSONbf=Cert_BCDF_JSON.txt
JSONgh=Cert_GH_JSON.txt
LUMI=36000

#if [ ! -f "$PILEUP_LATEST" ]; then
#   echo "File $PILEUP_LATEST does not exist on this site, copying from lxplus"
#   scp $USER@lxplus.cern.ch:$PILEUP_LATEST pileup_latest.txt
#   PILEUP_LATEST=pileup_latest.txt
#fi

echo $JSON
pileupCalc.py -i $JSON --inputLumiJSON $PILEUP_LATEST --calcMode true --minBiasXsec 62010 --maxPileupBin 100 --numPileupBins 1000 PU_2016_${LUMI}_XSecDown.root
echo $JSON
pileupCalc.py -i $JSON --inputLumiJSON $PILEUP_LATEST --calcMode true --minBiasXsec 65000 --maxPileupBin 100 --numPileupBins 1000 PU_2016_${LUMI}_XSecCentral.root
echo $JSON
pileupCalc.py -i $JSON --inputLumiJSON $PILEUP_LATEST --calcMode true --minBiasXsec 67990 --maxPileupBin 100 --numPileupBins 1000 PU_2016_${LUMI}_XSecUp.root
echo $JSONbf
pileupCalc.py -i $JSONbf --inputLumiJSON $PILEUP_LATEST --calcMode true --minBiasXsec 62010 --maxPileupBin 100 --numPileupBins 1000 PU_2016_BCDEF_XSecDown.root
echo $JSONbf
pileupCalc.py -i $JSONbf --inputLumiJSON $PILEUP_LATEST --calcMode true --minBiasXsec 65000 --maxPileupBin 100 --numPileupBins 1000 PU_2016_BCDEF_XSecCentral.root
echo $JSONbf
pileupCalc.py -i $JSONbf --inputLumiJSON $PILEUP_LATEST --calcMode true --minBiasXsec 67990 --maxPileupBin 100 --numPileupBins 1000 PU_2016_BCDEF_XSecUp.root
echo $JSONgh
pileupCalc.py -i $JSONgh --inputLumiJSON $PILEUP_LATEST --calcMode true --minBiasXsec 62010 --maxPileupBin 100 --numPileupBins 1000 PU_2016_GH_XSecDown.root
echo $JSONgh
pileupCalc.py -i $JSONgh --inputLumiJSON $PILEUP_LATEST --calcMode true --minBiasXsec 65000 --maxPileupBin 100 --numPileupBins 1000 PU_2016_GH_XSecCentral.root
echo $JSONgh
pileupCalc.py -i $JSONgh --inputLumiJSON $PILEUP_LATEST --calcMode true --minBiasXsec 67990 --maxPileupBin 100 --numPileupBins 1000 PU_2016_GH_XSecUp.root
