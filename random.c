#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <getopt.h>

#define MAX_CHOICES 100
#define MAX_CHOICE_LEN 256

typedef struct {
    char *choices[MAX_CHOICES];
    int count;
    int num_selections;
    int with_replacement;
    int show_all;
    int seed;
    int random_int_mode;
    int int_min;
    int int_max;
    int random_float_mode;
    int random_bool_mode;
} RandomChoicesConfig;

// Función para imprimir la ayuda
void print_help() {
    printf("=== Random Choices - Librería para aleatoriedad en Infernal ===\n\n");
    printf("Uso: ./random [OPCIONES]\n\n");
    printf("FUNCIONES DE SELECCIÓN:\n");
    printf("  -c, --choice <item> [<item> ...]\n");
    printf("                  Agregar opciones a elegir (puede poner varias seguidas)\n");
    printf("  -n, --num <número>       Cantidad de selecciones (default: 1)\n");
    printf("  -r, --replace            Permitir repetición (con reemplazo)\n");
    printf("\n");
    printf("FUNCIONES NUMÉRICAS:\n");
    printf("  -i, --randint <x> <y>    Entero aleatorio en [x, y] (inclusivo)\n");
    printf("  -f, --random             Float aleatorio en [0.0, 1.0)\n");
    printf("  -u, --uniform <x> <y>    Float aleatorio en [x, y)\n");
    printf("\n");
    printf("OTRAS FUNCIONES:\n");
    printf("  -b, --randombool         Booleano aleatorio (True/False)\n");
    printf("  -s, --seed <número>      Establecer seed para reproducibilidad\n");
    printf("  -a, --all                Mostrar todas las opciones disponibles\n");
    printf("  -h, --help               Mostrar esta ayuda\n");
    printf("      --version            Mostrar versión del programa\n");
    printf("      --edition            Mostrar edición del programa\n\n");
    printf("EJEMPLOS:\n");
    printf("  ./random -c piedra papel tijera\n");
    printf("  ./random -c \"una frase\" \"otra frase\" -r\n");
    printf("  ./random -i 1 100 -n 10\n");
    printf("  ./random -f -n 3\n");
    printf("  ./random -u 0.5 1.5 -n 2\n");
    printf("  ./random -b -n 5\n");
    printf("  ./random -c A B C -n 3 -s 42\n");
}

// Función para inicializar la configuración
RandomChoicesConfig* init_config() {
    RandomChoicesConfig *config = (RandomChoicesConfig *)malloc(sizeof(RandomChoicesConfig));
    config->count = 0;
    config->num_selections = 1;
    config->with_replacement = 0;
    config->show_all = 0;
    config->seed = -1;
    config->random_int_mode = 0;
    config->int_min = 0;
    config->int_max = 0;
    config->random_float_mode = 0;
    config->random_bool_mode = 0;
    return config;
}

// Función para agregar una opción
int add_choice(RandomChoicesConfig *config, const char *choice) {
    if (config->count >= MAX_CHOICES) {
        fprintf(stderr, "Error: Se alcanzó el máximo de opciones (%d)\n", MAX_CHOICES);
        return 0;
    }
    config->choices[config->count] = (char *)malloc(strlen(choice) + 1);
    strcpy(config->choices[config->count], choice);
    config->count++;
    return 1;
}

// Función para seleccionar aleatoriamente (sin reemplazo)
void select_without_replacement(RandomChoicesConfig *config) {
    if (config->num_selections > config->count) {
        fprintf(stderr, "ERROR: Imposible seleccionar %d elementos sin repetición. Máximo disponible: %d\n",
                config->num_selections, config->count);
        fprintf(stderr, "       Use -r o --replace para permitir repetición.\n");
        return;
    }

    // Crear array de índices
    int *indices = (int *)malloc(config->count * sizeof(int));
    for (int i = 0; i < config->count; i++) {
        indices[i] = i;
    }

    // Fisher-Yates shuffle
    for (int i = config->count - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = indices[i];
        indices[i] = indices[j];
        indices[j] = temp;
    }

    for (int i = 0; i < config->num_selections; i++) {
        printf("%s", config->choices[indices[i]]);
        if (i < config->num_selections - 1) {
            printf(" ");
        }
    }
    printf("\n");

    free(indices);
}

// Función para seleccionar aleatoriamente (con reemplazo)
void select_with_replacement(RandomChoicesConfig *config) {
    for (int i = 0; i < config->num_selections; i++) {
        int idx = rand() % config->count;
        printf("%s", config->choices[idx]);
        if (i < config->num_selections - 1) {
            printf(" ");
        }
    }
    printf("\n");
}

// Función para mostrar todas las opciones
void show_all_choices(RandomChoicesConfig *config) {
    printf("Opciones disponibles:\n");
    for (int i = 0; i < config->count; i++) {
        printf("  %d. %s\n", i + 1, config->choices[i]);
    }
}

// Función para generar números aleatorios enteros
void random_int_range(RandomChoicesConfig *config) {
    if (config->int_min > config->int_max) {
        fprintf(stderr, "ERROR: randint() requires a <= b. Got a=%d, b=%d\n",
                config->int_min, config->int_max);
        return;
    }

    int range = config->int_max - config->int_min + 1;

    for (int i = 0; i < config->num_selections; i++) {
        int random_num = config->int_min + (rand() % range);
        printf("%d", random_num);
        if (i < config->num_selections - 1) {
            printf(" ");
        }
    }
    printf("\n");
}

// Función para generar números aleatorios flotantes [0.0, 1.0)
void random_float(RandomChoicesConfig *config) {
    for (int i = 0; i < config->num_selections; i++) {
        double random_num = (double)rand() / RAND_MAX;
        printf("%.16f", random_num);
        if (i < config->num_selections - 1) {
            printf(" ");
        }
    }
    printf("\n");
}

// Función para generar números aleatorios flotantes en rango [a, b)
void random_uniform(RandomChoicesConfig *config) {
    if (config->int_min >= config->int_max) {
        fprintf(stderr, "ERROR: uniform() requires a < b. Got a=%d, b=%d\n",
                config->int_min, config->int_max);
        return;
    }

    for (int i = 0; i < config->num_selections; i++) {
        double range = (double)(config->int_max - config->int_min);
        double random_num = config->int_min + (range * ((double)rand() / RAND_MAX));
        printf("%.16f", random_num);
        if (i < config->num_selections - 1) {
            printf(" ");
        }
    }
    printf("\n");
}

// Función para generar booleano aleatorio
void random_bool(RandomChoicesConfig *config) {
    for (int i = 0; i < config->num_selections; i++) {
        int random_bool = rand() % 2;
        printf("%s", random_bool ? "True" : "False");
        if (i < config->num_selections - 1) {
            printf(" ");
        }
    }
    printf("\n");
}

// Función para liberar memoria
void free_config(RandomChoicesConfig *config) {
    for (int i = 0; i < config->count; i++) {
        free(config->choices[i]);
    }
    free(config);
}

int main(int argc, char *argv[]) {
    RandomChoicesConfig *config = init_config();

    // Definir opciones largas
    struct option long_options[] = {
        {"choice",    required_argument, 0, 'c'},
        {"num",       required_argument, 0, 'n'},
        {"replace",   no_argument,       0, 'r'},
        {"seed",      required_argument, 0, 's'},
        {"all",       no_argument,       0, 'a'},
        {"randint",   required_argument, 0, 'i'},
        {"random",    no_argument,       0, 'f'},
        {"uniform",   required_argument, 0, 'u'},
        {"randombool",no_argument,       0, 'b'},
        {"help",      no_argument,       0, 'h'},
        {"version",   no_argument,       0, 'v'},
        {"edition",   no_argument,       0, 'e'},
        {0, 0, 0, 0}
    };

    int option_index = 0;
    int c;

    // Parsear argumentos
    while ((c = getopt_long(argc, argv, "c:n:rs:ai:fu:bhve", long_options, &option_index)) != -1) {
        switch (c) {
            case 'c':
                // Primera opción (obligatoria con -c)
                if (!add_choice(config, optarg)) {
                    free_config(config);
                    return 1;
                }
                // Consumir todos los argumentos adicionales hasta la siguiente bandera
                while (optind < argc && argv[optind][0] != '-') {
                    if (!add_choice(config, argv[optind])) {
                        free_config(config);
                        return 1;
                    }
                    optind++;
                }
                break;
            case 'n':
                config->num_selections = atoi(optarg);
                if (config->num_selections <= 0) {
                    fprintf(stderr, "Error: El número de selecciones debe ser positivo\n");
                    free_config(config);
                    return 1;
                }
                break;
            case 'r':
                config->with_replacement = 1;
                break;
            case 's':
                config->seed = atoi(optarg);
                break;
            case 'a':
                config->show_all = 1;
                break;
            case 'i':
                config->random_int_mode = 1;
                config->int_min = atoi(optarg);
                if (optind < argc && argv[optind][0] != '-') {
                    config->int_max = atoi(argv[optind]);
                    optind++;
                } else {
                    fprintf(stderr, "ERROR: --randint requiere dos argumentos: <min> <max>\n");
                    fprintf(stderr, "       Uso: ./random --randint 1 100\n");
                    free_config(config);
                    return 1;
                }
                break;
            case 'f':
                config->random_float_mode = 1;
                break;
            case 'u':
                config->random_float_mode = 2;  // modo uniform
                config->int_min = atoi(optarg);  // Cuidado: atoi trunca floats
                if (optind < argc && argv[optind][0] != '-') {
                    config->int_max = atoi(argv[optind]);
                    optind++;
                } else {
                    fprintf(stderr, "ERROR: --uniform requiere dos argumentos: <min> <max>\n");
                    fprintf(stderr, "       Uso: ./random --uniform 0.5 1.5\n");
                    free_config(config);
                    return 1;
                }
                break;
            case 'b':
                config->random_bool_mode = 1;
                break;
            case 'v':
                printf("1.0\n");
                free_config(config);
                return 0;
            case 'e':
                printf("Lynds Corp., Librería Random para Infernal\n");
                printf("Código fuente: github.com/LyndsCorp/Infernal-Random\n");
                printf("Licencia: GPL-3.0-or-later\n");
                printf("Escrito por David Baña Szymaniak.\n");
                free_config(config);
                return 0;
            case 'h':
                print_help();
                free_config(config);
                return 0;
            case '?':
                fprintf(stderr, "Use -h o --help para ver la ayuda\n");
                free_config(config);
                return 1;
            default:
                break;
        }
    }

    // Contar cuántos modos están activos
    int float_mode_active = (config->random_float_mode > 0 ? 1 : 0);
    int active_modes = (config->count > 0 ? 1 : 0) +
    config->random_int_mode +
    float_mode_active +
    config->random_bool_mode;

    // Validar que solo un modo esté activo
    if (active_modes == 0) {
        fprintf(stderr, "ERROR: Debes elegir una función:\n");
        fprintf(stderr, "       -c/--choice (selección de opciones)\n");
        fprintf(stderr, "       -i/--randint (entero aleatorio)\n");
        fprintf(stderr, "       -f/--random (flotante aleatorio)\n");
        fprintf(stderr, "       -u/--uniform (flotante en rango)\n");
        fprintf(stderr, "       -b/--randombool (booleano aleatorio)\n");
        fprintf(stderr, "Use -h o --help para más información.\n");
        free_config(config);
        return 1;
    }

    if (active_modes > 1) {
        fprintf(stderr, "ERROR: Solo puedes usar una función a la vez.\n");
        fprintf(stderr, "       No puedes combinar -c, -i, -f, -u, -b en el mismo comando.\n");
        free_config(config);
        return 1;
    }

    // Inicializar seed
    if (config->seed >= 0) {
        srand(config->seed);
    } else {
        srand((unsigned int)time(NULL));
    }

    // Ejecutar según el modo
    if (config->random_int_mode) {
        random_int_range(config);
    } else if (config->random_float_mode == 1) {
        random_float(config);
    } else if (config->random_float_mode == 2) {
        random_uniform(config);
    } else if (config->random_bool_mode) {
        random_bool(config);
    } else {
        // Modo choices
        if (config->show_all) {
            show_all_choices(config);
            printf("\n");
        }
        if (config->with_replacement) {
            select_with_replacement(config);
        } else {
            select_without_replacement(config);
        }
    }

    free_config(config);
    return 0;
}
