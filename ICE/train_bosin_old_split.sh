echo "Start training embeddings for LyricsRec..."
GRAPH_PATH="/home/thwang/ICE/ICE/graph/split_graph/"
EMBD_PATH="./embd/"
for SAMP in 20000
do
    for NEG in 5
    do
        for TRAIN_INFO in "top10" "top8"
        do
            WW_EDGE=$GRAPH_PATH"ww_"$TRAIN_INFO"x3.edge"
            SW_EDGE=$GRAPH_PATH"sw_"$TRAIN_INFO"x3.edge"
            WORD_IN_EMBD=$EMBD_PATH"word_in-"$TRAIN_INFO"x3_w0_neg"$NEG".embd"
            WORD_OUT_EMBD=$EMBD_PATH"word_out-"$TRAIN_INFO"x3_w0_neg"$NEG".embd"
            SONG_IN_EMBD=$EMBD_PATH"song_in-"$TRAIN_INFO"x3_w0_neg"$NEG".embd"

            echo
            echo "===================================================================================="
            echo "Training Bosin's ICE:"
            echo "===================================================================================="
            echo

            echo $ICE_EMBD
            ./ice -text $WW_EDGE -entity $SW_EDGE -textrep $WORD_IN_EMBD -textcontext $WORD_OUT_EMBD -save $SONG_IN_EMBD -dim 300 -neg $NEG -sample $SAMP -thread 20 -alpha 0.025

        done
    done
done
echo "Completed training embeddings for LyricsRec."
