#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// Structure to store patient data and treatment details
typedef struct {
    int stage;
    int heart_health;
    int lung_health;
    int bulky_tumors;
    int num_cycles;
    int weeks_per_cycle;
    int break_weeks;
    int treatment_days;
    int break_days;
} PatientData;

// Function prototypes
void get_patient_data(PatientData *data);
void choose_treatment_method(PatientData *data);
void generate_initial_schedule(PatientData *data);
double cost_function(PatientData *data);
void simulated_annealing(PatientData *data);
void adjust_next_cycle(PatientData *data);
int ask_continue_treatment();

int main() {
    srand(time(NULL)); // Seed random number generator

    PatientData data;
    get_patient_data(&data);
    choose_treatment_method(&data);
    generate_initial_schedule(&data);
    
    printf("\nInitial Treatment Schedule:\n");
    printf("Number of cycles: %d\n", data.num_cycles);
    printf("Weeks per cycle: %d\n", data.weeks_per_cycle);
    printf("Break weeks between cycles: %d\n", data.break_weeks);
    printf("Treatment days in cycle 1: %d\n", data.treatment_days);
    printf("Break days in cycle 1: %d\n", data.break_days);

    // Perform simulated annealing to optimize the treatment schedule
    simulated_annealing(&data);

    printf("\nOptimized Schedule:\n");
    printf("Treatment days in final cycle: %d\n", data.treatment_days);
    printf("Break days in final cycle: %d\n", data.break_days);

    int cycle = 1;
    while (cycle <= data.num_cycles && ask_continue_treatment()) {
        printf("\n--- Cycle %d ---\n", cycle);
        adjust_next_cycle(&data);
        printf("Treatment days: %d\n", data.treatment_days);
        printf("Break days: %d\n", data.break_days);
        cycle++;
    }

    if (cycle > data.num_cycles) {
        printf("All cycles completed successfully.\n");
    } else {
        printf("Treatment discontinued after cycle %d.\n", cycle - 1);
    }

    return 0;
}

void get_patient_data(PatientData *data) {
    printf("Enter the stage of Hodgkin's Lymphoma (1-4): ");
    scanf("%d", &data->stage);

    printf("Enter the percentage of heart health (0-100): ");
    scanf("%d", &data->heart_health);

    printf("Enter the percentage of lung health (0-100): ");
    scanf("%d", &data->lung_health);

    printf("Is there bulky tumors or involvement of multiple lymph node groups? (1 for Yes, 0 for No): ");
    scanf("%d", &data->bulky_tumors);
}

void choose_treatment_method(PatientData *data) {
    if (data->stage <= 2 && data->heart_health > 60 && data->lung_health > 60 && !data->bulky_tumors) {
        printf("Treatment Method: ABVD\n");
        data->num_cycles = 6;
        data->weeks_per_cycle = 4;
        data->break_weeks = 1;
    } else if (data->stage <= 2 && data->heart_health > 50 && data->lung_health > 50 && data->bulky_tumors) {
        printf("Treatment Method: Stanford V\n");
        data->num_cycles = 12;
        data->weeks_per_cycle = 3;
        data->break_weeks = 0;
    } else if (data->stage > 2 && data->heart_health > 50 && data->lung_health > 50) {
        printf("Treatment Method: A + AVD\n");
        data->num_cycles = 4;
        data->weeks_per_cycle = 6;
        data->break_weeks = 2;
    } else {
        printf("Treatment Method: BEACOPP\n");
        data->num_cycles = 8;
        data->weeks_per_cycle = 3;
        data->break_weeks = 1;
    }
}

void generate_initial_schedule(PatientData *data) {
    data->treatment_days = data->weeks_per_cycle * 5; // Assume treatment on weekdays
    data->break_days = data->weeks_per_cycle * 2;     // Assume break on weekends
}

double cost_function(PatientData *data) {
    // Hypothetical cost function: Lower is better
    // Factors like number of treatment days and break days are considered
    return data->treatment_days * 1.0 + data->break_days * 0.5;
}

void simulated_annealing(PatientData *data) {
    double temp = 1000.0;
    double cooling_rate = 0.95;
    int max_iterations = 1000;

    PatientData current_data = *data;
    PatientData best_data = *data;
    double best_cost = cost_function(&best_data);

    for (int iteration = 0; iteration < max_iterations; iteration++) {
        temp *= cooling_rate;

        // Make a small random change to the schedule
        PatientData new_data = current_data;
        new_data.treatment_days += rand() % 5 - 2; // Change between -2 and +2 days
        new_data.break_days += rand() % 5 - 2;

        // Ensure treatment and break days are within reasonable bounds
        if (new_data.treatment_days < 0) new_data.treatment_days = 0;
        if (new_data.break_days < 0) new_data.break_days = 0;

        double new_cost = cost_function(&new_data);

        // Calculate acceptance probability
        double acceptance_prob = 0.0;
        if (new_cost < cost_function(&current_data)) {
            acceptance_prob = 1.0;
        } else {
            acceptance_prob = exp((cost_function(&current_data) - new_cost) / temp);
        }

        // Decide whether to accept the new schedule
        double random_prob = (double)rand() / RAND_MAX;
        if (random_prob < acceptance_prob) {
            current_data = new_data;

            // Update best schedule if new cost is lower
            if (new_cost < best_cost) {
                best_data = new_data;
                best_cost = new_cost;
            }
        }

        // Terminate early if temperature is sufficiently low
        if (temp < 1e-3) {
            break;
        }
    }

    *data = best_data;
}

void adjust_next_cycle(PatientData *data) {
    int tolerance, recovery, wbc, rbc, platelet_count, tumor_change, heart_health, lung_health;

    printf("Enter level of tolerance to treatment (0-100%%): ");
    scanf("%d", &tolerance);

    printf("Enter ability to recover from side effects (0-100%%): ");
    scanf("%d", &recovery);

    printf("Enter WBC count (in thousands per cubic millimeter): ");
    scanf("%d", &wbc);

    printf("Enter RBC count (in thousands per cubic millimeter): ");
    scanf("%d", &rbc);

    printf("Enter Platelet count (in thousands per cubic millimeter): ");
    scanf("%d", &platelet_count);

    printf("Enter tumor change (shrinkage: negative, growth: positive) in percentage: ");
    scanf("%d", &tumor_change);

    printf("Enter the percentage of heart health (0-100): ");
    scanf("%d", &heart_health);

    printf("Enter the percentage of lung health (0-100): ");
    scanf("%d", &lung_health);

    // Adjust treatment and break days based on patient response
    if (tolerance < 50 || recovery < 50 || wbc < 4 || rbc < 3 || platelet_count < 100 || heart_health < 50 || lung_health < 50) {
        data->treatment_days -= 2; // Reduce treatment days
        data->break_days += 2;     // Increase break days
    } 
    if (tumor_change < 0 && tolerance > 70 && recovery > 70) {
        data->treatment_days += 1; // Increase treatment days
        data->break_days -= 1;     // Decrease break days
    }

    // Ensure treatment and break days are within reasonable bounds
    if (data->treatment_days < 0) data->treatment_days = 0;
    if (data->break_days < 0) data->break_days = 0;
}

int ask_continue_treatment() {
    char response;
    printf("Do you want to continue with the treatment? (Y/N): ");
    scanf(" %c", &response);
    return (response == 'Y' || response == 'y');
}