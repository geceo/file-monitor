#define log_error(x,...) do { printf("%s: " ## x , ##_VA_ARGS__);
#define log_warning(x,...) do { printf("%s: " ## x , ##_VA_ARGS__);
#define log_message(x,...) do { printf("%s: " ## x , ##_VA_ARGS__);
