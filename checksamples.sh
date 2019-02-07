

for line in $(cat ./src/FCN/SampleList.cxx); #$(grep compare ../src/FCN/SampleList.cxx);
do
    
    if  [[ "$line" == *"compare"* ]];
    then
	parsed=${line/'!name.compare'/}
	parsed=${parsed/'('/}
	parsed=${parsed/')'/}
	parsed=${parsed/'"'/}
	parsed=${parsed/'("'/}
	parsed=${parsed/'")'/}
	echo $parsed
    fi

    if [[ "$line" == *"find("* ]];
    then
	parsed=${line/'!name.find'/}
	parsed=${parsed/'('/}
	parsed=${parsed/')'/}
	parsed=${parsed/'"'/}
	parsed=${parsed/'("'/}
	parsed=${parsed/'")'/}
	echo "MultIEventProcessor: $line"
    fi
   	
    
done;
