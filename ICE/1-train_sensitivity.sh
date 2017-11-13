echo "Start training embeddings for LyricsRec..."
GRAPH_PATH="./graph/"
EMBD_PATH="./embd/"
for COPY in "copy1/" "copy2/" "copy3/"
do
    for SAMP in 15000
    do
        for NEG in 5
        do
            for TRAIN_INFO in "top10"
            do
                for EXP in "x10" 
                do
                    for WEIGHT in "_w0"
                    do
                        ICE_EDGE=$GRAPH_PATH"ice_full-"$TRAIN_INFO$EXP$WEIGHT".edge"
                        ICE_EMBD=$EMBD_PATH$COPY"ice_full-"$TRAIN_INFO$EXP$WEIGHT".embd"

                        echo
                        echo "===================================================================================="
                        echo "Training Bosin's ICE:"
                        echo "Load: "$ICE_EDGE
                        echo "Save: "$ICE_EMBD
                        echo "===================================================================================="
                        echo
                        ./ice -text $ICE_EDGE -textrep $ICE_EMBD -save_times 20 -dim 300 -neg $NEG -sample $SAMP -thread 28 -alpha 0.025
                    done
                done
            done
        done
    done
done
echo "Completed training embeddings for LyricsRec."
