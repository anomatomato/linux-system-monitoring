#include "sys.h"
#include "common.h"
#include <sensors/sensors.h>
#include <stdio.h>
#include <string.h>

int sys() {
        char message[MAX_LINE];
        message[0] = '\0';

        char placeholder[MAX_BUFFER];
        placeholder[0] = '\0';

        /*char form[21][MAX_BUFFER] = {"sys-temp,chip=\0", "max=\0", "min=\0", "max_hyst=\0", "min_hyst=\0",
                                     "input=\0", "crit=\0", "crit_hyst=\0", "emergency=\0",
                                     "emergency_hyst=\0", "lcrit=\0", "lcrit_hyst=\0", "offset=\0",
                                     "label=\0", "lowest=\0", "highest=\0", "reset_history=\0",
                                     "reset_history_all=\0", "enable=\0", ",\0", " \0"};*/

        if (sensors_init(NULL) != 0) { /*chip liste wird geladen, NULL lädt den default*/
                sensors_cleanup();
                return 1;
        }
        empty_queue(); /*queue wird geleert, weil sensor_init die queue beeinflusst*/

        sensors_chip_name const *chip;
        int c = 0;

        while ((chip = sensors_get_detected_chips(0, &c)) != NULL) { /*loop um alle chips zu laden*/

                sprintf(message, "sys-temp,chip=%s ", chip->prefix);

                sensors_feature const *feat;
                int f = 0;
                int tempnr = 0;

                while ((feat = sensors_get_features(chip, &f)) != NULL) { /*loop für alle features*/
                        if (feat->type != SENSORS_FEATURE_TEMP) /*nur temperaturen sind von interesse*/
                                continue;

                        if (tempnr > 0) /*extra komma für das line protokol format*/
                                strcat(message, ",\0");
                        tempnr++;

                        sensors_subfeature const *sub;
                        int s = 0;

                        while ((sub = sensors_get_all_subfeatures(chip, feat, &s)) != NULL) { /*alle subfeats*/
                                if (sub->type != SENSORS_SUBFEATURE_TEMP_INPUT) /*input nur von interesse*/
                                        continue;

                                double val;
                                if (sub->flags && SENSORS_MODE_R) { /*wenn gelesen werden kann*/
                                        int rc = sensors_get_value(chip, sub->number, &val);

                                        if (rc <
                                            0) { /*falls der wert nicht gelesen werden konnte, wird 0 angehangen*/
                                                sprintf(placeholder, "%s=0.00", feat->name);
                                                strcat(message, placeholder);
                                                continue;
                                        }
                                        sprintf(placeholder, "%s=%.2lf", feat->name, val);
                                        strcat(message, placeholder);
                                }
                        }
                }
                if (strstr(message, ".") == NULL) {
                        message[0] = '\0';
                        continue;
                }

                strcat(message, " \0");
                if (enqueue(message) == 1) /*an die queue anhängen*/
                        return 1;

                message[0] = '\0'; /*message wiederverwenden*/
        }
        sensors_cleanup();
        return 0;
}
