#pragma once

int main()
{
    int i;
    int r;
    jsmn_parser p;
    jsmntok_t t[128]; /* We expect no more than 128 tokens */

    jsmn_init(&p);
    r = jsmn_parse(&p, JSON_STRING, strlen(JSON_STRING), t, sizeof(t) / sizeof(t[0]));

    if (r < 0)
    {
        printf("Failed to parse JSON: %d\n", r);
        return 1;
    }

    /* Assume the top-level element is an object */
    if (r < 1 || t[0].type != JSMN_OBJECT)
    {
        printf("Object expected\n");
        return 1;
    }

    /* Loop over all keys of the root object */
    for (i = 1; i < r; i++)
    {
        if (jsoneq(JSON_STRING, &t[i], "user") == 0)
        {
            /* We may use strndup() to fetch string value */
            printf("- User: %.*s\n", t[i + 1].end - t[i + 1].start,
                JSON_STRING + t[i + 1].start);
            i++;
        }
        else if (jsoneq(JSON_STRING, &t[i], "admin") == 0)
        {
            /* We may additionally check if the value is either "true" or "false" */
            printf("- Admin: %.*s\n", t[i + 1].end - t[i + 1].start,
                JSON_STRING + t[i + 1].start);
            i++;
        }
        else if (jsoneq(JSON_STRING, &t[i], "uid") == 0)
        {
            /* We may want to do strtol() here to get numeric value */
            printf("- UID: %.*s\n", t[i + 1].end - t[i + 1].start,
                JSON_STRING + t[i + 1].start);
            i++;
        }
        else if (jsoneq(JSON_STRING, &t[i], "groups") == 0)
        {
            int j;
            printf("- Groups:\n");
            if (t[i + 1].type != JSMN_ARRAY)
            {
                continue; /* We expect groups to be an array of strings */
            }
            for (j = 0; j < t[i + 1].size; j++)
            {
                jsmntok_t* g = &t[i + j + 2];
                printf("  * %.*s\n", g->end - g->start, JSON_STRING + g->start);
            }
            i += t[i + 1].size + 1;
        }
        else
        {
            printf("Unexpected key: %.*s\n", t[i].end - t[i].start,
                JSON_STRING + t[i].start);
        }
    }
    return EXIT_SUCCESS;
}
