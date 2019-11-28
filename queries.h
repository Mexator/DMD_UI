#ifndef QUERIES_H
#define QUERIES_H
#include <QString>
static QString query_str1 = QString("\
SELECT DISTINCT doctor, first_name, second_name FROM\n\
    (record JOIN employee ON record.doctor=employee.e_id) AS first_table\n\
WHERE\n\
    ((first_name ~* '^[ml]' AND second_name !~* '^[ml]')\n\
    OR (first_name !~* '^[ml]' AND second_name ~* '^[ml]'))\n\
AND\n\
    patient=\n\
    (SELECT p_id FROM patient\n\
    WHERE first_name=%1 AND second_name=%2 )\n\
AND EXTRACT(DOY FROM\n\
    (SELECT max(second_table.timestamp) FROM (record JOIN patient ON record.patient=patient.p_id) AS second_table\n\
    WHERE second_table.first_name=%1 AND second_table.second_name=%2 ))=\n\
    EXTRACT(DOY FROM first_table.timestamp)\n\
AND\n\
    EXTRACT(YEAR FROM\n\
    (SELECT max(third_table.timestamp) FROM (record JOIN patient ON record.patient=patient.p_id) AS third_table\n\
    WHERE third_table.first_name=%1 AND third_table.second_name=%2 ))=\n\
    EXTRACT(YEAR FROM first_table.timestamp);");
static QString query_str2 = QString("\
SELECT\n\
    doctor,\n\
    first_name,\n\
    second_name,\n\
    EXTRACT(DOW FROM timestamp)::INTEGER AS day_of_week,\n\
    EXTRACT(HOUR FROM timestamp)::INTEGER AS timeslot,\n\
    COUNT(*) AS total_appointments,\n\
    (COUNT(*) / 52.0)::double precision AS average_appointments\n\
FROM\n\
    record\n\
    JOIN employee\n\
    ON record.doctor=employee.e_id\n\
GROUP BY\n\
    doctor,\n\
    first_name,\n\
    second_name,\n\
    day_of_week,\n\
    timeslot\n\
ORDER BY\n\
    doctor ASC,\n\
    day_of_week ASC,\n\
    timeslot ASC;");
static QString query_str3 = QString("\
SELECT DISTINCT p_id, first_name, second_name \n\
FROM\n\
    patient JOIN record ON record.patient=patient.p_id\n\
WHERE\n\
    p_id IN\n\
        (SELECT patient FROM\n\
            (SELECT patient, count(patient) as count_1 FROM record\n\
            WHERE age(now(), timestamp) < INTERVAL '1 week'\n\
            GROUP BY patient) AS foo\n\
        WHERE\n\
            count_1 > 1)\n\
        AND p_id IN\n\
            (SELECT patient FROM\n\
                (SELECT patient, count(patient) as count_2 FROM record\n\
                WHERE age(now(), timestamp) < INTERVAL '2 week' AND\n\
                    age(now(), timestamp) >= INTERVAL '1 week'\n\
                GROUP BY patient) AS foo\n\
                WHERE\n\
                    count_2 > 1)\n\
        AND p_id IN\n\
            (SELECT patient FROM\n\
                (SELECT patient, count(patient) as count_3 FROM record\n\
                WHERE age(now(), timestamp) < INTERVAL '3 week' AND\n\
                    age(now(), timestamp) >= INTERVAL '2 week'\n\
                GROUP BY patient) AS foo\n\
                WHERE\n\
                    count_3 > 1)\n\
        AND p_id IN\n\
            (SELECT patient FROM\n\
                (SELECT patient, count(patient) as count_4 FROM record\n\
                WHERE age(now(), timestamp) < INTERVAL '4 week' AND\n\
                    age(now(), timestamp) >= INTERVAL '3 week'\n\
                GROUP BY patient) AS foo\n\
                WHERE\n\
                    count_4 > 1);\n");

static QString query_str4_1 = QString("\
CREATE OR REPLACE FUNCTION patient_age(patient INTEGER) RETURNS double precision AS $$\n\
    SELECT EXTRACT(YEAR from age(now(), birth_date)) FROM patient\n\
        WHERE p_id=$1;\n\
$$ LANGUAGE SQL;\n");
static QString query_str4_2=QString("\
SELECT \n\
    sum(cost) AS total_income\n\
FROM\n\
    (SELECT \n\
        (CASE WHEN count(patient) <  3 AND patient_age(patient) <  50 THEN count(patient)*200\n\
             WHEN count(patient) >= 3 AND patient_age(patient) <  50 THEN count(patient)*250\n\
             WHEN count(patient) <  3 AND patient_age(patient) >= 50 THEN count(patient)*400\n\
             WHEN count(patient) >= 3 AND patient_age(patient) >= 50 THEN count(patient)*500\n\
             ELSE 0\n\
        END) AS cost\n\
    FROM\n\
        (SELECT patient FROM record\n\
        WHERE age(now(), timestamp) <= INTERVAL '1 month') AS last_month_patients\n\
    GROUP BY patient) AS costs;");
static QString query_str5_1 = QString("\
CREATE OR REPLACE FUNCTION doctor_report() RETURNS TABLE (\n\
    id INTEGER,\n\
    first_name VARCHAR,\n\
    second_name VARCHAR) AS $$\n\
DECLARE\n\
    i integer := 0;\n\
BEGIN\n\
    CREATE TEMP TABLE res (\n\
        id INTEGER,\n\
        first_name VARCHAR,\n\
        second_name VARCHAR);\n\
    INSERT INTO res\n\
        (SELECT table_2.doctor, table_2.first_name, table_2.second_name FROM\n\
            ((SELECT doctor, count(doctor) AS appointmets_count FROM record GROUP BY doctor) AS table_1\n\
            JOIN employee ON table_1.doctor=employee.e_id) AS table_2\n\
        WHERE appointmets_count >= 100);\n\
    LOOP\n\
        DELETE FROM res\n\
        WHERE res.id NOT IN\n\
            (SELECT doctor AS id FROM\n\
                (SELECT doctor, count(doctor) AS count_doctor FROM\n\
                    (SELECT * FROM record\n\
                    WHERE age(now(), record.timestamp) <  format('%s year', i+1)::interval AND\n\
                        age(now(), record.timestamp) >= format('%s year', i)::interval) AS samiy_vnutrenniy\n\
                    GROUP BY doctor) AS ne_samiy_vnutrenniy\n\
                WHERE count_doctor >= 5);\n\
        i := i + 1; \n\
        EXIT WHEN i = 10;\n\
    END LOOP;\n\
RETURN QUERY SELECT * FROM res;\n\
END; $$ LANGUAGE plpgsql;\n");
static QString query_str5_2 = QString("\
SELECT * FROM doctor_report();\n");
#endif // QUERIES_H
