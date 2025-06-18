#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <float.h>

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif

typedef struct {
    int weight;
    double value;
} Item;

void generate_random_data(int n, Item items[]) {
    srand(time(NULL));
    for (int i = 0; i < n; i++) {
        items[i].weight = rand() % 100 + 1;
        items[i].value = (rand() % 900 + 100) / 100.0;
    }
}

double knapsack_brute_force(int n, int capacity, Item items[]) {
    double max_value = 0.0;
    for (int mask = 0; mask < (1 << n); mask++) {
        int current_weight = 0;
        double current_value = 0.0;
        for (int i = 0; i < n; i++) {
            if (mask & (1 << i)) {
                current_weight += items[i].weight;
                current_value += items[i].value;
            }
        }
        if (current_weight <= capacity && current_value > max_value) {
            max_value = current_value;
        }
    }
    return max_value;
}

double knapsack_dynamic(int n, int capacity, Item items[]) {
    double *dp = (double *)malloc((capacity + 1) * sizeof(double));
    for (int i = 0; i <= capacity; i++) dp[i] = 0.0;
    for (int i = 0; i < n; i++) {
        for (int w = capacity; w >= items[i].weight; w--) {
            if (dp[w - items[i].weight] + items[i].value > dp[w]) {
                dp[w] = dp[w - items[i].weight] + items[i].value;
            }
        }
    }
    double result = dp[capacity];
    free(dp);
    return result;
}

double knapsack_greedy(int n, int capacity, Item items[]) {
    double density[n];
    for (int i = 0; i < n; i++) {
        density[i] = items[i].value / items[i].weight;
    }
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            if (density[i] < density[j]) {
                double temp = density[i];
                density[i] = density[j];
                density[j] = temp;
                Item tmp = items[i];
                items[i] = items[j];
                items[j] = tmp;
            }
        }
    }
    int remaining_capacity = capacity;
    double total_value = 0.0;
    for (int i = 0; i < n && remaining_capacity > 0; i++) {
        if (items[i].weight <= remaining_capacity) {
            total_value += items[i].value;
            remaining_capacity -= items[i].weight;
        }
    }
    return total_value;
}

void knapsack_backtracking(int n, int capacity, Item items[], int index, int curr_weight, double curr_value, double *max_value) {
    if (curr_weight > capacity) return;
    if (index == n) {
        if (curr_value > *max_value) *max_value = curr_value;
        return;
    }
    knapsack_backtracking(n, capacity, items, index + 1, curr_weight + items[index].weight, curr_value + items[index].value, max_value);
    knapsack_backtracking(n, capacity, items, index + 1, curr_weight, curr_value, max_value);
}

void generate_appendix_data(int n, int capacity, Item items[], const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) return;
    fprintf(file, "number,weight,value\n");
    for (int i = 0; i < n; i++) {
        fprintf(file, "%d,%d,%.2f\n", i + 1, items[i].weight, items[i].value);
    }
    fclose(file);
}

bool file_exists(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file) { fclose(file); return true; }
    return false;
}

int main() {
    int n_values[] = {5, 10, 15, 20, 1000, 2000, 3000, 4000, 5000,
                      6000, 7000, 8000, 9000, 10000, 20000, 40000, 80000, 160000, 320000};
    int capacity_values[] = {10000, 100000, 1000000};
    int num_n = sizeof(n_values) / sizeof(n_values[0]);
    int num_capacity = sizeof(capacity_values) / sizeof(capacity_values[0]);

    const char *output_dir = "C:\\Users\\Lenovo\\Desktop";
#ifdef _WIN32
    _mkdir(output_dir);
#else
    mkdir(output_dir, 0777);
#endif

    const char *csv_path = "C:\\Users\\Lenovo\\Desktop\\results.csv";
    bool write_header = !file_exists(csv_path);
    FILE *csv = fopen(csv_path, "a");
    if (!csv) {
        printf("Failed to open CSV file\n");
        return 1;
    }
    if (write_header) {
        fprintf(csv, "n,capacity,brute_force_result,brute_force_time,dynamic_result,dynamic_time,greedy_result,greedy_time,backtracking_result,backtracking_time\n");
    }

    for (int i = 0; i < num_n; i++) {
        int n = n_values[i];
        for (int j = 0; j < num_capacity; j++) {
            int capacity = capacity_values[j];
            Item *items = (Item *)malloc(n * sizeof(Item));
            generate_random_data(n, items);

            if (n == 1000) {
                char appendix_file[256];
                sprintf(appendix_file, "%s\\appendix_data.csv", output_dir);
                generate_appendix_data(n, capacity, items, appendix_file);
            }

            double brute_force_result = 0.0, brute_time = 0.0;
            if (n <= 20) {
                clock_t start = clock();
                brute_force_result = knapsack_brute_force(n, capacity, items);
                clock_t end = clock();
                brute_time = (double)(end - start) / CLOCKS_PER_SEC * 1000;
                printf("[Brute]     n=%d cap=%d val=%.2f time=%.2fms\n", n, capacity, brute_force_result, brute_time);
            }

            clock_t start = clock();
            double dynamic_result = knapsack_dynamic(n, capacity, items);
            clock_t end = clock();
            double dynamic_time = (double)(end - start) / CLOCKS_PER_SEC * 1000;
            printf("[Dynamic]   n=%d cap=%d val=%.2f time=%.2fms\n", n, capacity, dynamic_result, dynamic_time);

            start = clock();
            double greedy_result = knapsack_greedy(n, capacity, items);
            end = clock();
            double greedy_time = (double)(end - start) / CLOCKS_PER_SEC * 1000;
            printf("[Greedy]    n=%d cap=%d val=%.2f time=%.2fms\n", n, capacity, greedy_result, greedy_time);

            double backtracking_result = 0.0, backtracking_time = 0.0;
            if (n <= 20) {
                double max_value = 0.0;
                start = clock();
                knapsack_backtracking(n, capacity, items, 0, 0, 0.0, &max_value);
                end = clock();
                backtracking_result = max_value;
                backtracking_time = (double)(end - start) / CLOCKS_PER_SEC * 1000;
                printf("[Backtrack] n=%d cap=%d val=%.2f time=%.2fms\n", n, capacity, backtracking_result, backtracking_time);
            }

            fprintf(csv, "%d,%d,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n",
                    n, capacity,
                    brute_force_result, brute_time,
                    dynamic_result, dynamic_time,
                    greedy_result, greedy_time,
                    backtracking_result, backtracking_time);

            free(items);
        }
    }

    fclose(csv);
    printf("Finished writing results.csv\n");
    return 0;
}
