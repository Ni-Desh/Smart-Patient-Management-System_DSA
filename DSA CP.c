#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

// Data Structures
typedef struct Appointment {
    char date[20];
    char time[10];
    char doctorName[100];
    char hospital[100];
    struct Appointment* next;
} Appointment;

typedef struct Patient {
    int id;
    char name[100];
    int age;
    char disease[100];
    float bill;
    Appointment* history;
    struct Patient* next;
} Patient;

typedef struct Doctor {
    char id[10];           // Doctor ID (e.g., D100)
    char name[100];
    char specialty[100];
    char hospital[100];
    char schedule[7][9][15]; // 7 dates × 9 timeslots
    struct Doctor* next;
} Doctor;

int doctorIDCounter = 100;  // Static counter for Doctor IDs

// Emergency Queue
typedef struct EmergencyNode {
    char patientName[100];
    int patientID;  // Added to store patient ID
    struct EmergencyNode* next;
} EmergencyNode;

EmergencyNode* emergencyFront = NULL;
EmergencyNode* emergencyRear = NULL;

Doctor* doctorHead = NULL;
Patient* patientHead = NULL;
int patientIDCounter = 100;

const char* timeSlots[] = {
    "10:00 AM", "11:30 AM", "1:00 PM", "2:30 PM", "4:00 PM",
    "5:30 PM", "6:00 PM", "7:15 PM", "8:30 PM"
};
int totalTimeSlots = 9;

char globalDates[7][20];

void stripNewline(char* str) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') str[len - 1] = '\0';
}

void enqueueEmergency(const char* name, int id) {
    EmergencyNode* newNode = (EmergencyNode*)malloc(sizeof(EmergencyNode));
    strcpy(newNode->patientName, name);
    newNode->patientID = id;  // Store patient ID
    newNode->next = NULL;
    if (emergencyRear == NULL) {
        emergencyFront = emergencyRear = newNode;
    } else {
        emergencyRear->next = newNode;
        emergencyRear = newNode;
    }
}

void showPendingEmergencies() {
    if (emergencyFront == NULL) {
        printf("No pending emergency cases.\n");
        return;
    }

    printf("\n\033[1;31mPending Emergency Cases:\033[0m\n");
    EmergencyNode* temp = emergencyFront;
    int count = 1;
    while (temp) {
        printf("%d. %s (ID: %d)\n", count++, temp->patientName, temp->patientID);
        temp = temp->next;
    }
}

void handleEmergency() {
    int id;
    printf("Enter patient ID for emergency: ");
    scanf("%d", &id);

    Patient* pat = patientHead;
    while (pat && pat->id != id) pat = pat->next;

    if (!pat) {
        printf("Patient not found! Please register the patient first.\n");
        return;
    }

    enqueueEmergency(pat->name, pat->id);
    printf("\033[5;31mEmergency case added:\033[0m %s (ID: %d)\n", pat->name, pat->id);

    // Show current queue status
    showPendingEmergencies();
}

void dequeueEmergency() {
    if (emergencyFront == NULL) {
        printf("No emergency patients in queue.\n");
        return;
    }

    printf("\033[5;31mEMERGENCY ALERT!\033[0m\n");  // Blinking red text
    printf("Processing emergency patient: \033[1;31m%s (ID: %d)\033[0m\n",
           emergencyFront->patientName, emergencyFront->patientID);

    // Show pending emergencies if any
    if (emergencyFront->next != NULL) {
        printf("\n\033[1;33mPending Emergency Cases:\033[0m\n");
        EmergencyNode* temp = emergencyFront->next;
        int count = 1;
        while (temp) {
            printf("%d. %s (ID: %d)\n", count++, temp->patientName, temp->patientID);
            temp = temp->next;
        }
    } else {
        printf("No other pending emergency cases.\n");
    }

    EmergencyNode* temp = emergencyFront;
    emergencyFront = emergencyFront->next;
    if (emergencyFront == NULL) emergencyRear = NULL;
    free(temp);
}

void displayDates(char dates[7][20]) {
    time_t now = time(NULL);
    struct tm t = *localtime(&now);
    for (int i = 0; i < 7; i++) {
        t.tm_mday = localtime(&now)->tm_mday + i;
        mktime(&t);
        strftime(dates[i], 20, "%Y-%m-%d", &t);
    }
    memcpy(globalDates, dates, sizeof(globalDates)); // Save for reference in matrix
}

void addDoctor(const char* name, const char* specialty, const char* hospital) {
    Doctor* newDoc = (Doctor*)malloc(sizeof(Doctor));
    // Generate Doctor ID
    sprintf(newDoc->id, "D%d", doctorIDCounter++);

    strcpy(newDoc->name, name);
    strcpy(newDoc->specialty, specialty);
    strcpy(newDoc->hospital, hospital);

    // Initialize the schedule matrix
    for (int i = 0; i < 7; i++)
        for (int j = 0; j < totalTimeSlots; j++)
            strcpy(newDoc->schedule[i][j], "A");

    newDoc->next = doctorHead;
    doctorHead = newDoc;
    printf("Doctor added with ID: %s\n", newDoc->id);
}


void preloadDoctors() {
    char dates[7][20];
    displayDates(dates);
    addDoctor("Dr. Seema Deshmukh", "Cardiologist", "HeartCare Hospital");
    addDoctor("Dr. Amit Rathi", "General Physician", "HealthPlus Pune");
    addDoctor("Dr. Meenal Joshi", "Dermatologist", "SkinCare Clinic");
}

void deleteDoctor() {
    char doctorID[10];
    getchar();
    printf("Enter doctor's ID to delete (e.g., D100): ");
    fgets(doctorID, sizeof(doctorID), stdin);
    stripNewline(doctorID);

    Doctor* temp = doctorHead;
    Doctor* prev = NULL;
    while (temp) {
        if (strcmp(temp->id, doctorID) == 0) {
            if (prev) prev->next = temp->next;
            else doctorHead = temp->next;
            free(temp);
            printf("Doctor with ID %s deleted successfully.\n", doctorID);
            return;
        }
        prev = temp;
        temp = temp->next;
    }
    printf("Doctor with ID %s not found.\n", doctorID);
}


void viewDoctors() {
    Doctor* temp = doctorHead;
    printf("\nList of Doctors:\n");
    while (temp) {
        printf("ID: %s |  %s (%s) - %s\n", temp->id, temp->name, temp->specialty, temp->hospital);
        temp = temp->next;
    }
}


void deletePatient() {
    int id;
    printf("Enter patient ID to delete: ");
    scanf("%d", &id);

    Patient* temp = patientHead;
    Patient* prev = NULL;
    while (temp) {
        if (temp->id == id) {
            if (prev) prev->next = temp->next;
            else patientHead = temp->next;
            free(temp);
            printf("Patient deleted successfully.\n");
            return;
        }
        prev = temp;
        temp = temp->next;
    }
    printf("Patient not found.\n");
}

void viewPatients() {
    Patient* temp = patientHead;
    printf("\nList of Patients:\n");
    while (temp) {
        printf("ID: %d | Name: %s | Age: %d | Disease: %s\n", temp->id, temp->name, temp->age, temp->disease);
        temp = temp->next;
    }
}

void addPatient() {
    Patient* newP = (Patient*)malloc(sizeof(Patient));
    getchar();
    printf("Enter patient name: ");
    fgets(newP->name, sizeof(newP->name), stdin);
    stripNewline(newP->name);
    printf("Enter age: ");
    scanf("%d", &newP->age);
    getchar();
    printf("Enter disease: ");
    fgets(newP->disease, sizeof(newP->disease), stdin);
    stripNewline(newP->disease);
    newP->bill = 0;
    newP->history = NULL;
    newP->id = patientIDCounter++;
    newP->next = patientHead;
    patientHead = newP;
    printf("Patient ID: %d\n", newP->id);
}

Doctor* findDoctorByName(const char* name) {
    Doctor* d = doctorHead;
    while (d) {
        if (strcmp(d->name, name) == 0) return d;
        d = d->next;
    }
    return NULL;
}

void bookAppointment() {
    int id;
    printf("Enter patient ID: ");
    scanf("%d", &id);

    Patient* pat = patientHead;
    while (pat && pat->id != id) pat = pat->next;
    if (!pat) {
        printf("Patient not found!\n");
        return;
    }

    char dates[7][20];
    displayDates(dates);
    printf("\nAvailable Dates:\n");
    for (int i = 0; i < 7; i++) {
        printf("%d. %s\n", i + 1, dates[i]);
    }
    int dateChoice;
    printf("Choose a date (1-7): ");
    scanf("%d", &dateChoice);
    if (dateChoice < 1 || dateChoice > 7) {
        printf("Invalid date choice.\n");
        return;
    }

    Doctor* doc = doctorHead;
    int docNum = 1;
    Doctor* docArray[20];
    printf("\033[1;36mPlease select a doctor from the list below:\033[0m\n");
    while (doc) {
        printf("%d.  %s (%s) - %s\n", docNum, doc->name, doc->specialty, doc->hospital);
        docArray[docNum - 1] = doc;
        doc = doc->next;
        docNum++;
    }

    int docChoice;
    printf("Choose doctor number: ");
    scanf("%d", &docChoice);
    if (docChoice < 1 || docChoice >= docNum) {
        printf("\033[1;30;43mInvalid Doctor Choice!\033[0m\n");

        return;
    }
    Doctor* selectedDoc = docArray[docChoice - 1];

    printf("\nAvailable Time Slots for Dr. %s on %s:\n", selectedDoc->name, dates[dateChoice - 1]);
    int slotNum = 1;
    const char* availableSlots[9];
    for (int i = 0; i < totalTimeSlots; i++) {
        if (strcmp(selectedDoc->schedule[dateChoice - 1][i], "A") == 0) {
            availableSlots[slotNum - 1] = timeSlots[i];
            printf("%d. %s\n", slotNum++, timeSlots[i]);
        }
    }

    if (slotNum == 1) {
        printf("No available slots.\n");
        return;
    }

    int slotChoice;
    printf("Choose slot number: ");
    scanf("%d", &slotChoice);
    if (slotChoice < 1 || slotChoice >= slotNum) {
        printf("Invalid slot choice.\n");
        return;
    }

    Appointment* newApp = (Appointment*)malloc(sizeof(Appointment));
    strcpy(newApp->date, dates[dateChoice - 1]);
    strcpy(newApp->time, availableSlots[slotChoice - 1]);
    strcpy(newApp->doctorName, selectedDoc->name);
    strcpy(newApp->hospital, selectedDoc->hospital);
    newApp->next = pat->history;
    pat->history = newApp;
    pat->bill += 500;

    // Update doctor schedule matrix
    for (int i = 0; i < totalTimeSlots; i++) {
        if (strcmp(timeSlots[i], newApp->time) == 0) {
            strcpy(selectedDoc->schedule[dateChoice - 1][i], "B");
            break;
        }
    }

    printf("\033[1;32mAppointment Confirmed!\033[0m\n");

    printf("Patient: %s\nDoctor: %s (%s)\nHospital: %s\nDate: %s\nTime: %s\n",
           pat->name, selectedDoc->name, selectedDoc->specialty,
           selectedDoc->hospital, newApp->date, newApp->time);
}

void viewHistory() {
    int id;
    printf("Enter patient ID to view history: ");
    scanf("%d", &id);
    Patient* pat = patientHead;
    while (pat && pat->id != id) pat = pat->next;
    if (!pat) {
        printf("Patient not found!\n");
        return;
    }

    printf("\nHistory for %s:\n", pat->name);
    Appointment* a = pat->history;
    if (!a) {
        printf("No appointments found.\n");
        return;
    }
    while (a) {
        printf("Date: %s | Time: %s | Doctor: %s | Hospital: %s\n",
               a->date, a->time, a->doctorName, a->hospital);
        a = a->next;
    }
    printf("Total Bill: Rs %.2f\n", pat->bill);
}


void viewDoctorSchedule() {
    char docID[10];
    getchar(); // consume newline from previous input
    printf("Enter Doctor ID to view schedule (e.g., D100): ");
    fgets(docID, sizeof(docID), stdin);
    docID[strcspn(docID, "\n")] = '\0';  // remove trailing newline

    // Inline findDoctorByID logic
    Doctor* doc = doctorHead;
    while (doc != NULL) {
        if (strcmp(doc->id, docID) == 0)
            break;
        doc = doc->next;
    }

    if (!doc) {
        printf("Doctor with ID %s not found.\n", docID);
        return;
    }

    printf("\n\033[1;34mSchedule for  %s (%s)\033[0m\n", doc->name, doc->id);

    // Print time slot headers
    printf("%-12s", "Date\\Time");
    for (int i = 0; i < totalTimeSlots; i++) {
        printf("%-12s", timeSlots[i]);
    }
    printf("\n");

    // Print matrix rows with dates and availability in color
    for (int i = 0; i < 7; i++) {
        printf("%-12s", globalDates[i]);  // date
        for (int j = 0; j < totalTimeSlots; j++) {
            if (strcmp(doc->schedule[i][j], "B") == 0) {
                printf("\033[1;31m%-12s\033[0m", "B"); // Red for Booked
            } else {
                printf("\033[1;32m%-12s\033[0m", "A"); // Green for Available
            }
        }
        printf("\n");
    }
}

int main() {

    preloadDoctors();

    int choice;
    do {
        printf("\033[1;35mSMART PATIENT MANAGEMENT SYSTEM\033[0m\n");
        printf("1. Add Doctor\n2. Add Patient\n3. Book Appointment\n4. View History\n");
        printf("5. Handle Emergency\n6. Process Emergency\n7. View Doctors\n8. View Patients\n");
        printf("9. Delete Doctor\n10. Delete Patient\n11. View Doctor Schedule\n12. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        switch (choice) {
            case 1: {
                char name[100], specialty[100], hospital[100];
                getchar();
                printf("Enter doctor name: ");
                fgets(name, sizeof(name), stdin);
                stripNewline(name);
                printf("Enter specialty (Options: MBBS, BHMS, CARDIOLOGIST, DERMATOLOGIST, GENERAL):");
                fgets(specialty, sizeof(specialty), stdin);
                stripNewline(specialty);
                printf("Enter hospital: ");
                fgets(hospital, sizeof(hospital), stdin);
                stripNewline(hospital);
                addDoctor(name, specialty, hospital);
                break;
            }
            case 2: addPatient(); break;
            case 3: bookAppointment(); break;
            case 4: viewHistory(); break;
            case 5: handleEmergency(); break;
            case 6: dequeueEmergency(); break;
            case 7: viewDoctors(); break;
            case 8: viewPatients(); break;
            case 9: deleteDoctor(); break;
            case 10: deletePatient(); break;
            case 11: viewDoctorSchedule(); break;
            case 12: printf("Exiting...\n"); break;
            default: printf("Invalid choice!\n");
        }
    } while (choice != 12);

    return 0;
}

