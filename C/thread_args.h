
struct Thread_args {
    pthread_mutex_t mutex;
    pthread_cond_t condition;
    int compteur;
};