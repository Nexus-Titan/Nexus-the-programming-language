#define _USE_MATH_DEFINES
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef _WIN32
#include <direct.h>
#include <windows.h>
#define popen _popen
#define pclose _pclose
#define getcwd _getcwd
#else
#include <unistd.h>
#endif
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#ifndef M_E
#define M_E 2.71828182845904523536
#endif
#define MAX_VARS 256
#define MAX_FUNCS 64
#define MAX_LINE 1024
#define MAX_NAME 64
#define MAX_LINES 1000

typedef struct {
  char name[MAX_NAME];
  char value[MAX_LINE];
} Variable;

typedef struct {
  char name[MAX_NAME];
  char lines[MAX_LINES][MAX_LINE];
  int line_count;
} Function;

Variable vars[MAX_VARS];
int var_count = 0;

Function funcs[MAX_FUNCS];
int func_count = 0;

char *trim(char *str) {
  char *end;
  while (isspace((unsigned char)*str))
    str++;
  if (*str == 0)
    return str;
  end = str + strlen(str) - 1;
  while (end > str && isspace((unsigned char)*end))
    end--;
  end[1] = '\0';
  return str;
}

void str_upper(char *str) {
  for (int i = 0; str[i]; i++)
    str[i] = toupper(str[i]);
}

void str_lower(char *str) {
  for (int i = 0; str[i]; i++)
    str[i] = tolower(str[i]);
}

void str_reverse(char *str) {
  int len = strlen(str);
  for (int i = 0; i < len / 2; i++) {
    char temp = str[i];
    str[i] = str[len - i - 1];
    str[len - i - 1] = temp;
  }
}

char *get_var(const char *name) {
  for (int i = 0; i < var_count; i++) {
    if (strcmp(vars[i].name, name) == 0) {
      return vars[i].value;
    }
  }
  return "";
}

void set_var(const char *name, const char *value) {
  for (int i = 0; i < var_count; i++) {
    if (strcmp(vars[i].name, name) == 0) {
      strncpy(vars[i].value, value, MAX_LINE - 1);
      return;
    }
  }
  if (var_count < MAX_VARS) {
    strncpy(vars[var_count].name, name, MAX_NAME - 1);
    strncpy(vars[var_count].value, value, MAX_LINE - 1);
    var_count++;
  }
}

void resolve_value(const char *input, char *output) {
  char temp[MAX_LINE];
  strncpy(temp, input, MAX_LINE - 1);
  char *trimmed = trim(temp);

  if (trimmed[0] == '"' && trimmed[strlen(trimmed) - 1] == '"') {
    strncpy(output, trimmed + 1, strlen(trimmed) - 2);
    output[strlen(trimmed) - 2] = '\0';
    return;
  }

  if (strstr(trimmed, "math.pi()")) {
    sprintf(output, "%.10f", M_PI);
    return;
  }

  if (strstr(trimmed, "math.e()")) {
    sprintf(output, "%.10f", M_E);
    return;
  }

  if (strstr(trimmed, "math.tau()")) {
    sprintf(output, "%.10f", 2.0 * M_PI);
    return;
  }

  if (strncmp(trimmed, "math.", 5) == 0) {
    char func_name[32];
    char arg_str[MAX_LINE];

    if (sscanf(trimmed, "math.%[^(](%[^)])", func_name, arg_str) == 2) {
      char resolved_arg[MAX_LINE];
      resolve_value(arg_str, resolved_arg);
      double val = atof(resolved_arg);
      double result = 0;

      if (strcmp(func_name, "sin") == 0)
        result = sin(val);
      else if (strcmp(func_name, "cos") == 0)
        result = cos(val);
      else if (strcmp(func_name, "tan") == 0)
        result = tan(val);
      else if (strcmp(func_name, "asin") == 0)
        result = asin(val);
      else if (strcmp(func_name, "acos") == 0)
        result = acos(val);
      else if (strcmp(func_name, "atan") == 0)
        result = atan(val);
      else if (strcmp(func_name, "sinh") == 0)
        result = sinh(val);
      else if (strcmp(func_name, "cosh") == 0)
        result = cosh(val);
      else if (strcmp(func_name, "tanh") == 0)
        result = tanh(val);
      else if (strcmp(func_name, "sqrt") == 0)
        result = sqrt(val);
      else if (strcmp(func_name, "cbrt") == 0)
        result = cbrt(val);
      else if (strcmp(func_name, "abs") == 0)
        result = fabs(val);
      else if (strcmp(func_name, "ceil") == 0)
        result = ceil(val);
      else if (strcmp(func_name, "floor") == 0)
        result = floor(val);
      else if (strcmp(func_name, "round") == 0)
        result = round(val);
      else if (strcmp(func_name, "log") == 0)
        result = log(val);
      else if (strcmp(func_name, "log10") == 0)
        result = log10(val);
      else if (strcmp(func_name, "log2") == 0)
        result = log2(val);
      else if (strcmp(func_name, "exp") == 0)
        result = exp(val);
      else if (strcmp(func_name, "trunc") == 0)
        result = trunc(val);
      else if (strcmp(func_name, "deg") == 0)
        result = val * 180.0 / M_PI;
      else if (strcmp(func_name, "rad") == 0)
        result = val * M_PI / 180.0;

      sprintf(output, "%.10f", result);
      return;
    }

    if (strncmp(trimmed, "math.pow(", 9) == 0) {
      char arg1[MAX_LINE], arg2[MAX_LINE];
      if (sscanf(trimmed, "math.pow(%[^,], %[^)])", arg1, arg2) == 2) {
        char res1[MAX_LINE], res2[MAX_LINE];
        resolve_value(arg1, res1);
        resolve_value(arg2, res2);
        double result = pow(atof(res1), atof(res2));
        sprintf(output, "%.10f", result);
        return;
      }
    }
  }

  if (strncmp(trimmed, "str.", 4) == 0) {
    char func_name[32];
    char arg_str[MAX_LINE];

    if (sscanf(trimmed, "str.%[^(](%[^)])", func_name, arg_str) == 2) {
      char resolved_arg[MAX_LINE];
      resolve_value(arg_str, resolved_arg);

      if (strcmp(func_name, "len") == 0) {
        sprintf(output, "%zu", strlen(resolved_arg));
        return;
      } else if (strcmp(func_name, "upper") == 0) {
        strcpy(output, resolved_arg);
        str_upper(output);
        return;
      } else if (strcmp(func_name, "lower") == 0) {
        strcpy(output, resolved_arg);
        str_lower(output);
        return;
      } else if (strcmp(func_name, "reverse") == 0) {
        strcpy(output, resolved_arg);
        str_reverse(output);
        return;
      } else if (strcmp(func_name, "trim") == 0) {
        strcpy(output, trim(resolved_arg));
        return;
      }
    }
  }

  if (strncmp(trimmed, "sys.", 4) == 0) {
    if (strcmp(trimmed, "sys.os()") == 0) {
#ifdef _WIN32
      strcpy(output, "Windows");
#else
      strcpy(output, "Linux");
#endif
      return;
    } else if (strcmp(trimmed, "sys.user()") == 0) {
#ifdef _WIN32
      char *user = getenv("USERNAME");
#else
      char *user = getenv("USER");
#endif
      strcpy(output, user ? user : "User");
      return;
    } else if (strcmp(trimmed, "sys.time()") == 0) {
      sprintf(output, "%ld", (long)time(NULL));
      return;
    } else if (strcmp(trimmed, "sys.cwd()") == 0) {
      char cwd[MAX_LINE];
      if (getcwd(cwd, sizeof(cwd)) != 0) {
        strcpy(output, cwd);
      }
      return;
    }
  }

  if (strncmp(trimmed, "date.", 5) == 0) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    if (strcmp(trimmed, "date.now()") == 0) {
      strftime(output, MAX_LINE, "%Y-%m-%d %H:%M:%S", t);
      return;
    } else if (strcmp(trimmed, "date.year()") == 0) {
      sprintf(output, "%d", t->tm_year + 1900);
      return;
    } else if (strcmp(trimmed, "date.month()") == 0) {
      sprintf(output, "%d", t->tm_mon + 1);
      return;
    } else if (strcmp(trimmed, "date.day()") == 0) {
      sprintf(output, "%d", t->tm_mday);
      return;
    } else if (strcmp(trimmed, "date.hour()") == 0) {
      sprintf(output, "%d", t->tm_hour);
      return;
    } else if (strcmp(trimmed, "date.min()") == 0) {
      sprintf(output, "%d", t->tm_min);
      return;
    } else if (strcmp(trimmed, "date.sec()") == 0) {
      sprintf(output, "%d", t->tm_sec);
      return;
    }
  }

  if (strncmp(trimmed, "rnd.", 4) == 0) {
    if (strcmp(trimmed, "rnd.val()") == 0) {
      sprintf(output, "%.6f", (double)rand() / RAND_MAX);
      return;
    } else if (strncmp(trimmed, "rnd.int(", 8) == 0) {
      int min, max;
      if (sscanf(trimmed, "rnd.int(%d, %d)", &min, &max) == 2) {
        int val = min + rand() % (max - min + 1);
        sprintf(output, "%d", val);
        return;
      }
    }
  }

  if (strncmp(trimmed, "io.read(", 8) == 0) {
    char filename[MAX_LINE];
    if (sscanf(trimmed, "io.read(%[^)])", filename) == 1) {
      char resolved_file[MAX_LINE];
      resolve_value(filename, resolved_file);

      FILE *f = fopen(resolved_file, "r");
      if (f) {
        char line[MAX_LINE];
        output[0] = '\0';
        while (fgets(line, sizeof(line), f)) {
          strcat(output, line);
          strcat(output, " | ");
        }
        fclose(f);
        return;
      }
    }
    strcpy(output, "Error");
    return;
  }

  if (strncmp(trimmed, "io.exists(", 10) == 0) {
    char filename[MAX_LINE];
    if (sscanf(trimmed, "io.exists(%[^)])", filename) == 1) {
      char resolved_file[MAX_LINE];
      resolve_value(filename, resolved_file);
      FILE *f = fopen(resolved_file, "r");
      if (f) {
        fclose(f);
        strcpy(output, "1");
      } else {
        strcpy(output, "0");
      }
      return;
    }
  }


  if (strncmp(trimmed, "admin.is_admin()", 16) == 0) {
#ifdef _WIN32
    strcpy(output, "0");
#else
    sprintf(output, "%d", geteuid() == 0);
#endif
    return;
  }
  if (strncmp(trimmed, "browser.open(", 13) == 0) {
    char url[MAX_LINE];
    if (sscanf(trimmed, "browser.open(%[^)])", url) == 1) {
      char resolved_url[MAX_LINE];
      resolve_value(url, resolved_url);
      char cmd[MAX_LINE * 2];
#ifdef _WIN32
      sprintf(cmd, "start %s", resolved_url);
#elif __APPLE__
      sprintf(cmd, "open %s", resolved_url);
#else
      sprintf(cmd, "xdg-open %s", resolved_url);
#endif
      system(cmd);
      strcpy(output, "1");
    }
    return;
  }
  if (strncmp(trimmed, "zip.extract(", 12) == 0) {
    char src[MAX_LINE], dst[MAX_LINE];
    if (sscanf(trimmed, "zip.extract(%[^,], %[^)])", src, dst) == 2) {
      char rsrc[MAX_LINE], rdst[MAX_LINE];
      resolve_value(src, rsrc);
      resolve_value(trim(dst), rdst);
      char cmd[MAX_LINE * 3];
      sprintf(cmd, "unzip -o %s -d %s", rsrc, rdst);
      system(cmd);
      strcpy(output, "1");
    }
    return;
  }

  char *var_val = get_var(trimmed);
  if (strlen(var_val) > 0) {
    strcpy(output, var_val);
    return;
  }

  if (isdigit(trimmed[0]) || (trimmed[0] == '-' && isdigit(trimmed[1]))) {
    strcpy(output, trimmed);
    return;
  }

  char *plus = strchr(trimmed, '+');
  if (plus) {
    char left[MAX_LINE], right[MAX_LINE];
    strncpy(left, trimmed, plus - trimmed);
    left[plus - trimmed] = '\0';
    strcpy(right, plus + 1);

    char left_val[MAX_LINE], right_val[MAX_LINE];
    resolve_value(left, left_val);
    resolve_value(right, right_val);

    sprintf(output, "%s%s", left_val, right_val);
    return;
  }

  strcpy(output, trimmed);
}

void run_nexus(char lines[][MAX_LINE], int line_count);

void execute_line(const char *line) {
  char temp[MAX_LINE];
  strncpy(temp, line, MAX_LINE - 1);
  char *trimmed = trim(temp);

  if (strlen(trimmed) == 0 || trimmed[0] == '#')
    return;

  if (strncmp(trimmed, "out ", 4) == 0) {
    char value[MAX_LINE];
    resolve_value(trimmed + 4, value);
    printf("Nexus › %s\n", value);
    return;
  }

  if (strncmp(trimmed, "set ", 4) == 0) {
    char var_name[MAX_NAME];
    char *equals = strchr(trimmed + 4, '=');
    if (equals) {
      strncpy(var_name, trimmed + 4, equals - (trimmed + 4));
      var_name[equals - (trimmed + 4)] = '\0';
      char *name_trimmed = trim(var_name);

      char value[MAX_LINE];
      resolve_value(equals + 1, value);
      set_var(name_trimmed, value);
    }
    return;
  }

  if (strncmp(trimmed, "wait ", 5) == 0) {
    char value[MAX_LINE];
    resolve_value(trimmed + 5, value);
    int seconds = atoi(value);
#ifdef _WIN32
    Sleep(seconds * 1000);
#else
    sleep(seconds);
#endif
    return;
  }

  if (strncmp(trimmed, "input ", 6) == 0) {
    char var_name[MAX_NAME];
    char prompt[MAX_LINE];

    if (sscanf(trimmed, "input %s %[^\n]", var_name, prompt) >= 1) {
      if (strlen(prompt) > 0) {
        char resolved_prompt[MAX_LINE];
        resolve_value(prompt, resolved_prompt);
        printf("%s: ", resolved_prompt);
      }

      char input[MAX_LINE];
      if (fgets(input, sizeof(input), stdin)) {
        input[strcspn(input, "\n")] = 0;
        set_var(var_name, input);
      }
    }
    return;
  }

  if (strncmp(trimmed, "gui.msg(", 8) == 0) {
    char arg[MAX_LINE];
    if (sscanf(trimmed, "gui.msg(%[^)])", arg) == 1) {
      char value[MAX_LINE];
      resolve_value(arg, value);
      printf("[GUI MESSAGE] %s\n", value);
    }
    return;
  }

  if (strncmp(trimmed, "io.write(", 9) == 0) {
    char filename[MAX_LINE], content[MAX_LINE];
    char *comma = strchr(trimmed + 9, ',');
    if (comma) {
      strncpy(filename, trimmed + 9, comma - (trimmed + 9));
      filename[comma - (trimmed + 9)] = '\0';

      char *end = strrchr(comma + 1, ')');
      if (end) {
        strncpy(content, comma + 1, end - (comma + 1));
        content[end - (comma + 1)] = '\0';

        char resolved_file[MAX_LINE], resolved_content[MAX_LINE];
        resolve_value(filename, resolved_file);
        resolve_value(content, resolved_content);

        FILE *f = fopen(resolved_file, "w");
        if (f) {
          fprintf(f, "%s\n", resolved_content);
          fclose(f);
        }
      }
    }
    return;
  }

  if (strcmp(trimmed, "sys.info()") == 0) {
    printf("Nexus › Nexus OS | v3.0-NEXUS | C-Engine\n");
    return;
  }
}

void run_nexus(char lines[][MAX_LINE], int line_count) {
  int i = 0;
  while (i < line_count) {
    char *line = trim(lines[i]);

    if (strncmp(line, "fn ", 3) == 0) {
      char func_name[MAX_NAME];
      sscanf(line, "fn %[^(]", func_name);

      if (func_count < MAX_FUNCS) {
        strcpy(funcs[func_count].name, func_name);
        funcs[func_count].line_count = 0;

        i++;
        while (i < line_count && strcmp(trim(lines[i]), "end") != 0) {
          strcpy(funcs[func_count].lines[funcs[func_count].line_count],
                 lines[i]);
          funcs[func_count].line_count++;
          i++;
        }
        func_count++;
      }
      i++;
      continue;
    }

    if (strncmp(line, "if ", 3) == 0) {
      char condition[MAX_LINE];
      strcpy(condition, line + 3);

      char cond_value[MAX_LINE];
      resolve_value(condition, cond_value);
      int cond_true = (atoi(cond_value) != 0 || strcmp(cond_value, "1") == 0);

      i++;
      int depth = 1;
      int else_pos = -1;
      int start = i;

      while (i < line_count && depth > 0) {
        char *current = trim(lines[i]);
        if (strncmp(current, "if ", 3) == 0)
          depth++;
        else if (strcmp(current, "else") == 0 && depth == 1)
          else_pos = i;
        else if (strcmp(current, "end") == 0)
          depth--;
        i++;
      }

      if (cond_true) {
        int end = (else_pos != -1) ? else_pos : i - 1;
        char block[MAX_LINES][MAX_LINE];
        int block_count = 0;
        for (int j = start; j < end; j++) {
          strcpy(block[block_count++], lines[j]);
        }
        run_nexus(block, block_count);
      } else if (else_pos != -1) {
        char block[MAX_LINES][MAX_LINE];
        int block_count = 0;
        for (int j = else_pos + 1; j < i - 1; j++) {
          strcpy(block[block_count++], lines[j]);
        }
        run_nexus(block, block_count);
      }
      continue;
    }

    if (strncmp(line, "loop ", 5) == 0) {
      char count_str[MAX_LINE];
      strcpy(count_str, line + 5);

      char count_value[MAX_LINE];
      resolve_value(count_str, count_value);
      int count = atoi(count_value);

      i++;
      int depth = 1;
      int start = i;

      while (i < line_count && depth > 0) {
        char *current = trim(lines[i]);
        if (strncmp(current, "loop ", 5) == 0)
          depth++;
        else if (strcmp(current, "end") == 0)
          depth--;
        i++;
      }

      char block[MAX_LINES][MAX_LINE];
      int block_count = 0;
      for (int j = start; j < i - 1; j++) {
        strcpy(block[block_count++], lines[j]);
      }

      for (int c = 0; c < count; c++) {
        run_nexus(block, block_count);
      }
      continue;
    }

    if (strstr(line, "()")) {
      char func_name[MAX_NAME];
      sscanf(line, "%[^(]", func_name);

      for (int f = 0; f < func_count; f++) {
        if (strcmp(funcs[f].name, func_name) == 0) {
          run_nexus(funcs[f].lines, funcs[f].line_count);
          break;
        }
      }
      i++;
      continue;
    }

    execute_line(line);
    i++;
  }
}

int main(int argc, char *argv[]) {
  srand(time(NULL));

  if (argc < 2) {
    printf("Nexus Titan Language v3.0-NEXUS (C)\n");
    printf("Usage: nexus <script.nx>\n");
    return 0;
  }

  FILE *file = fopen(argv[1], "r");
  if (!file) {
    printf("Error: Could not open file %s\n", argv[1]);
    return 1;
  }

  char lines[MAX_LINES][MAX_LINE];
  int line_count = 0;

  while (fgets(lines[line_count], MAX_LINE, file) && line_count < MAX_LINES) {
    line_count++;
  }
  fclose(file);

  run_nexus(lines, line_count);

  return 0;
}
