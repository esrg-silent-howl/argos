# run from the root of argos

cd "4. Implementation/2. Software Implementation/1. Main" && grep -rl silent . | xargs sed -i 's/josep/silent/g' && cd ../../..
