#define _GLIBCXX_USE_CXX11_ABI 1
#include "./ICE.h"

int ArgPos(char *str, int argc, char **argv) {
    int a;
    for (a = 1; a < argc; a++) if (!strcmp(str, argv[a])) {
        if ((a == argc - 1) || (*argv[a+1]=='-')) {
            printf("Argument missing for %s\n", str);
            exit(1);
        }
        return a;
    }
    return -1;
}

int main(int argc, char **argv){
    
    int i;

    if (argc == 1) {
        printf("[ICE-CLI]\n");
        printf("\tcommand line interface for ICE\n\n");
        printf("Options:\n");
        // printf("\t-train <string>\n");
        // printf("\t\tTrain the Network data\n");
        printf("\t-text <string>\n");
        printf("\t\ttext-text network data\n");
        printf("\t-entity <string>\n");
        printf("\t\tentity-text network data\n");
        printf("\t-textrep <string>\n");
        printf("\t\tsave the word representation data\n");
        printf("\t-textcontext <string>\n");
        printf("\t\tsave the word context data\n");
        printf("\t-save <string>\n");
        printf("\t\tSave the entity representation data\n");
        printf("\t-save_times <int>\n");
        printf("\t\tsave a model <int> times; default is 1\n");
        printf("\t-dim <int>\n");
        printf("\t\tDimension of vertex representation; default is 64\n");
        printf("\t-neg <int>\n");
        printf("\t\tNumber of negative examples; default is 5\n");
        printf("\t-sample <int>\n");
        printf("\t\tNumber of training samples *Million; default is 10\n");
        printf("\t-thread <int>\n");
        printf("\t\tNumber of training threads; default is 1\n");
        printf("\t-alpha <float>\n");
        printf("\t\tInit learning rate; default is 0.025\n");
        printf("\nExample Usage:\n");
        printf("\t./ice -text ../data/word.txt -entity ../data/entity.txt -textrep ../data/text.embd -save ../data/result.txt -textcontext ../data/context.txt -dim 4 -sample 10 -neg 5 -alpha 0.025 -thread 4\n\n");
        return 0;
    }
    
    char text_file[100], text_rep_file[100], text_context_file[100], entity_file[100], result_file[100];
    int dimensions=64, negative_samples=5, sample_times=10, save_times=1, threads=1;
    double init_alpha=0.025;

    // if ((i = ArgPos((char *)"-train", argc, argv)) > 0) strcpy(network_file, argv[i + 1]);
    // if ((i = ArgPos((char *)"-save", argc, argv)) > 0) strcpy(rep_file, argv[i + 1]);
    if ((i = ArgPos((char *)"-text", argc, argv)) > 0) strcpy(text_file, argv[i + 1]);
    if ((i = ArgPos((char *)"-entity", argc, argv)) > 0) strcpy(entity_file, argv[i + 1]);
    if ((i = ArgPos((char *)"-textrep", argc, argv)) > 0) strcpy(text_rep_file, argv[i + 1]);
    if ((i = ArgPos((char *)"-textcontext", argc, argv)) > 0) strcpy(text_context_file, argv[i + 1]);
    if ((i = ArgPos((char *)"-save", argc, argv)) > 0) strcpy(result_file, argv[i + 1]);
    if ((i = ArgPos((char *)"-save_times", argc, argv)) > 0) save_times = atoi(argv[i + 1]);
    if ((i = ArgPos((char *)"-dim", argc, argv)) > 0) dimensions = atoi(argv[i + 1]);
    if ((i = ArgPos((char *)"-sample", argc, argv)) > 0) sample_times = atoi(argv[i + 1]);
    if ((i = ArgPos((char *)"-neg", argc, argv)) > 0) negative_samples = atoi(argv[i + 1]);
    if ((i = ArgPos((char *)"-alpha", argc, argv)) > 0) init_alpha = atof(argv[i + 1]);
    if ((i = ArgPos((char *)"-thread", argc, argv)) > 0) threads = atoi(argv[i + 1]);

    ICE *ice;
    ice = new ICE();
    ice->LoadEdgeList(text_file);
    ice->Init(dimensions);
    ice->getVocabCount();

    int output_sample_list[] = {10, 20, 40, 80, 160, 320, 640, 1280, 2560, 5000,
        6000, 7000, 8000, 9000, 10000, 11000, 12000, 13000, 14000, 15000};
    int output_sample_list_size = sizeof(output_sample_list)/sizeof(int);

    double alpha_max=init_alpha, alpha_min=init_alpha;
    for (int i=0; i<output_sample_list_size; i++)
    {
        int output_sample = output_sample_list[i];
        int sample_interval = (i==0 ? output_sample:output_sample-output_sample_list[i-1]);

        alpha_max = alpha_min;
        alpha_min = init_alpha*((double)(sample_times-output_sample)/sample_times);
        if (alpha_min < init_alpha*0.0001) alpha_min = init_alpha*0.0001;

        ice->TrainStage1(sample_interval, negative_samples, alpha_max, alpha_min, threads);

        string sub_text_rep_file = text_rep_file + string(".") + to_string(output_sample);
        ice->SaveVertexWeights(sub_text_rep_file);

        string sub_text_context_file = text_context_file + string(".") + to_string(output_sample);
        ice->SaveContextWeights(sub_text_context_file);
    }

    ice->LoadEdgeList(entity_file);
    ice->Init(dimensions);
    ice->LoadWeights(text_context_file);

    alpha_max=init_alpha, alpha_min=init_alpha;
    for (int i=0; i<output_sample_list_size; i++)
    {
        int output_sample = output_sample_list[i];
        int sample_interval = (i==0 ? output_sample:output_sample-output_sample_list[i-1]);

        alpha_max = alpha_min;
        alpha_min = init_alpha*((double)(sample_times-output_sample)/sample_times);
        if (alpha_min < init_alpha*0.0001) alpha_min = init_alpha*0.0001;

        ice->TrainStage2(sample_interval, negative_samples, alpha_max, alpha_min, threads, ice->vocab_count);

        string sub_result_file = result_file + string(".") + to_string(output_sample);
        ice->SaveEntityWeights(sub_result_file);
    }

    return 0;

}
