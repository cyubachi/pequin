IFS_BK=$IFS
IFS='
'

i=0
for l in `head -50 result.txt`
do
  echo "bin/pepper_prover_logging  prove logging.pkey logging.inputs logging.outputs logging.proof $l"
  sh -c "bin/pepper_prover_logging  prove logging.pkey logging.inputs logging.outputs logging.proof $l"  > /dev/null 2>&1 &
  sleep 1
done

echo $i

IFS=$IFS_BK

