#include <stdio.h> 
#include <windows.h> 
#include <time.h> 
#include <stdlib.h> 
#include <string.h> 
#include <conio.h>

// --- Struct Definitions ---
struct song { 
    char *songname; 
    char *mood; 
    char *command; 
}; 
// Placeholders for your unimplemented functions
void defaultplaylist(struct song data[9]);
void createplaylist(struct song data[9]);
void moodplaylist(struct song data[9]); 
void queue(struct song data[9]);
void pickasong(struct song data[9]); 

struct node{
    struct song sdata;
    struct node *prev;
    struct node *next;
};
struct list{
    struct node *head;
    int size;
};
void initlist(struct list *ll){
     ll->head=NULL;
     ll->size=0;
}
void insertnode(struct list *ll, struct song s){
   struct node *newnode=(struct node*)malloc (sizeof(struct node));
   newnode->sdata=s;
   newnode->next=NULL;
   newnode->prev=NULL;

   if(ll->head==NULL)
     ll->head=newnode;
   else{
         struct node *temp=ll->head;
         while(temp->next!=NULL){
            temp=temp->next;
         }
         temp->next=newnode;
         newnode->prev=temp;
         }
         ll->size++;

}
void insertsongaftercurrent(struct list *ll,struct node *current,struct song s){
     struct node *newnode=(struct node*)malloc (sizeof(struct node));
   newnode->sdata=s;
   newnode->next=NULL;
   newnode->prev=NULL; 
    if(current->next!=NULL){
    newnode->next=current->next;
    newnode->prev=current;
    current->next->prev=newnode;
    current->next=newnode;
    }
    else{
        current->next=newnode;
        newnode->prev=current;
        newnode->next=NULL;
    }
}
void deletenode(struct node** current, struct list *ll){
 
    if (current == NULL || *current == NULL || ll->head == NULL) return;

    struct node *toDelete = *current;

    // Adjust head if deleting first node
    if (toDelete == ll->head) {
        ll->head = toDelete->next;
    }

    // Adjust surrounding pointers
    if (toDelete->next != NULL) {
        toDelete->next->prev = toDelete->prev;
    }
    if (toDelete->prev != NULL) {
        toDelete->prev->next = toDelete->next;
    }

    // Determine what node becomes the new 'current' track
    if (toDelete->next != NULL) {
        *current = toDelete->next;
    } else {
        *current = ll->head; // Fallback to start if deleting last song
    }

    free(toDelete);
    ll->size--;

}
void printlist(struct list *ll){
    int i=1;
    struct node *current = ll->head;
    if(current==NULL){
        printf("The playlist is empty\n");
        return;
    }
    while(current!=NULL){
        printf("%d. | %s\n",i,current->sdata.songname);
        current=current->next;
        i++;
    }
}
void playnode(struct node *n){
    printf("Playing %s.....\n", n->sdata.songname);
    mciSendString("close music", NULL, 0, NULL);
    mciSendString(n->sdata.command, NULL, 0, NULL);
    mciSendString("play music", NULL, 0, NULL);
}
int waitforchoice(void){
    char status[32];
    while(_kbhit()) _getch();              // clear any old key presses
    while(1){
        if(_kbhit()){
            int c = _getch();
            if(c == '1' || c == '2') return c - '0';
        }
        mciSendString("status music mode", status, sizeof(status), NULL);
        if(strcmp(status, "stopped") == 0) return 0;   // song finished, no input
        Sleep(100);
    }
}

void reverseplaylist(struct node **current){
    int rev, choice;
    struct node *node = *current;

    while(node->next != NULL) node = node->next;   // go to last song (d)
    playnode(node);

    printf("Do you want to continue the reverse playlist? Yes(1)/No(0)\n");
    scanf("%d", &rev);
    if(rev != 1){
        mciSendString("close music", NULL, 0, NULL);
        *current = node;
        return;
    }

    while(node->prev != NULL){
        printf("\n1. Next song (reverse)\n");
        printf("2. Go to play menu\n");
        choice = waitforchoice();

        if(choice == 2){
            mciSendString("close music", NULL, 0, NULL);
            break;
        }
        // choice == 1 (pressed) or 0 (song ended): both go to the previous node
        node = node->prev;
        playnode(node);
    }
    if(node->prev == NULL) printf("Reached the first song of the playlist\n");
    *current = node;
}
int comparesongs(struct song *a, struct song *b, int by) {
    if (by == 1) return _stricmp(a->songname, b->songname);
    if (by == 2) return _stricmp(a->mood, b->mood);
}

/* Remove one node from its current position */
void removeNode(struct list *ll, struct node *node)
{   struct node* current = ll->head;
    while(current->next!=NULL) current=current->next;
    if (node->prev != NULL)
        node->prev->next = node->next;
    else
        ll->head = node->next;

    if (node->next != NULL)
        node->next->prev = node->prev;
    else
        current = node->prev;

    node->prev = NULL;
    node->next = NULL;
}

void insertBefore(struct list *ll, struct node *node, struct node *before)
{
    node->prev = before->prev;
    node->next = before;

    if (before->prev != NULL)
        before->prev->next = node;
    else
        ll->head = node;

    before->prev = node;
}


void sortlist(struct list *ll, int by)
{
    struct node *current;
    struct node *scan;
    struct node *smallest;

    if (ll->head == NULL || ll->head->next == NULL)
        return;

    current = ll->head;

    while (current != NULL)
    {
        smallest = current;
        scan = current->next;
        while (scan != NULL)
        {
            if (comparesongs(&scan->sdata,&smallest->sdata,by) < 0)
                smallest = scan;

            scan = scan->next;
        }

        if (smallest != current)
        {
            removeNode(ll, smallest);
            insertBefore(ll, smallest, current);
        }
        else
            current = current->next;
    }
}
void play(struct list *ll,struct song data[9]){
    int ch,sign,i,sno,by;
    struct node *current=ll->head;
    if(current==NULL)
    {printf("The playlist is empty\n");
     return ;
    }
        do{
            printf("Playing %s.....\n",current->sdata.songname);
        mciSendString("close music",NULL,0,NULL);
        mciSendString(current->sdata.command,NULL,0,NULL);
        mciSendString("play music",NULL,0,NULL);
            printf("1.Next song\n");
            printf("2.Previous song\n");
            printf("3.Insert a song after current song\n");
            printf("4.Delete current song\n");
            printf("5.Reverse the playlist\n");
            printf("6.Sort the node of data of list\n");
            printf("7.Stop playing and go back to menu\n");
            printf("Please enter your choice\n");
            scanf("%d",&ch);
            switch(ch){
                case 1: if(current->next!=NULL)
                        {
                         current=current->next;
                         break;
                        }
                         else
                         printf("This is the last song of the playlist\n");
                         break;
                case 2: if(current->prev!=NULL)
                        {
                        current=current->prev;
                        break;
                        }
                        else
                        printf("This is the first song\n");
                        break;
                case 3: for(i=0;i<9;i++)
                        printf("%d. | %s \n",(i+1),data[i].songname);
                        printf("Enter the serial number of the song to be added next\n");
                        scanf("%d",&sno);
                        insertsongaftercurrent(ll,current,data[sno-1]);
                        printlist(ll);
                        break;
                case 4: deletenode(&current,ll);
                        printlist(ll);
                        if(current==NULL)
                          ch=7;
                        mciSendString("close music",NULL,0,NULL);
                        break;
                case 5: reverseplaylist(&current);
                        break;
                case 6: // sort alphabet order
                      printf("Sort by: 1.Song name  2.Mood \nEnter choice: ");
                      scanf("%d",&by);
                      if (by >= 1 && by <= 2) {
                      sortlist(ll, by);
                      current = ll->head;
                      //changed = 1;
                      printf("Playlist sorted.\n");
                      printlist(ll);
                     } else {
                     printf("Invalid choice\n");
                     }
                break;
                case 7: mciSendString("close music",NULL,0,NULL);
                       printf("Going back to main menu\n");
                       break;
                default:printf("Invalid choice\n");
                    }
        }while(ch!=7);
}

int main() { 
    int choice;
    int contd =1;
    
    // Array for song database 
    struct song data[9] = { 
        {"Senorita", "Romantic", "open \"C:\\Users\\admin\\Downloads\\Senorita.mp3.mpeg\" type mpegvideo alias music"}, 
        {"Dandelion", "Sad", "open \"C:\\Users\\admin\\Downloads\\dandelion.mp3.mpeg\" type mpegvideo alias music"},  
        {"Naatu Naatu", "Energitic", "open \"C:\\Users\\admin\\Downloads\\naatunaatu.mp3.mpeg\" type mpegvideo alias music"} ,
        {"I ain't worried ", "Feel-good", "open \"C:\\Users\\admin\\Downloads\\Iaintworried.mp3.mpeg\" type mpegvideo alias music"}, 
        {"Muskaanein Jhooti Hai", "Sad", "open \"C:\\Users\\admin\\Downloads\\muskanjooth.mp3.mpeg\" type mpegvideo alias music"},
         {"Why this Kolaveri Di", "Sad", "open \"C:\\Users\\admin\\Downloads\\kolaveri.mp3.mpeg\" type mpegvideo alias music"},
         {"Dynamite", "Feel-good", "open \"C:\\Users\\admin\\Downloads\\dynamite.mp3.mpeg\" type mpegvideo alias music"},
         {"Minnale from Amaran", "Romantic", "open \"C:\\Users\\admin\\Downloads\\minnale.mp3.mpeg\" type mpegvideo alias music"},
         {"Heart Attack", "Sad", "open \"C:\\Users\\admin\\Downloads\\heartattack.mp3.mpeg\" type mpegvideo alias music"}   
     }; 
     printf("PITCH PERFECT\n");
    do { 
        printf("\n------------MENU-----------\n"); 
        printf("1. Default playlist\n"); 
        printf("2. Create your own playlist\n"); 
        printf("3. Select playlist based on your mood\n"); 
        printf("4. Queue\n"); 
        printf("5. Pick a song\n"); 
        printf("6. Exit\n"); 
        printf("Enter your choice: "); 
        scanf("%d", &choice); 

        switch(choice) { 
            case 1: defaultplaylist(data); break; 
            case 2: createplaylist(data); break; 
            case 3: moodplaylist(data); break; 
            case 4: queue(data); break; 
            case 5: pickasong(data); break; 
            case 6: 
                printf("Pay your next visit to hearloss centre\n"); 
                return 0;
                break;
            default: 
                printf("Invalid choice\n"); 
        } 
        
            printf("\nDo you want to continue? Yes(1)/ No(0): "); 
            scanf("%d", &contd); 
    } while(contd != 0); 

    printf("Thank you for visiting\n"); 
    return 0; 
} 
void defaultplaylist(struct song data[9]){
                int i;
                struct list mixed;
                initlist(&mixed);
                srand(time(NULL));
                for(i=0;i<9;i++){
                    insertnode(&mixed, data[rand()%9]);
                }
                printf("MIXED PLAYLIST\n");
                printlist(&mixed);
                play(&mixed,data);

}

void moodplaylist(struct song data[9]) { 
    int choice,i; 
    printf("\nPick a theme among the following themes:\n"); 
    printf("1. Romantic\n"); 
    printf("2. Feel-good\n");
    printf("3.Sad\n");  
    printf("Enter choice: ");
    scanf("%d", &choice); 

    switch(choice){
        case 1: {
                struct list rom;
                initlist(&rom);
                for(i=0; i<9; i++){
                   if(strcmp(data[i].mood, "Romantic")==0) insertnode(&rom, data[i]);
                }
                play(&rom,data);
                }
        break;
        case 2: {
                struct list feelgood;
                initlist(&feelgood);
                for(i=0; i<9; i++){
                   if(strcmp(data[i].mood, "Feel-good")==0) insertnode(&feelgood, data[i]);
                }
                play(&feelgood,data);
                }
        break;
        case 3:{
                struct list sad;
                initlist(&sad);
                for(i=0; i<9; i++){
                   if(strcmp(data[i].mood, "Sad")==0) insertnode(&sad, data[i]);
                }
                play(&sad,data);
                }
                break;
        default:printf("Invalid choice\n");
        }
    } 
void createplaylist(struct song data[9]){
    int i,ch,sno;
    char name[50];
    struct list mylist;
    initlist(&mylist);
    mylist.size=0;
    //displaying database
    for(i=0;i<9;i++){
        printf("%d. | %s\n",(i+1),data[i].songname);
    }
    printf("Please enter the name of your playlist\n");
    while ((getchar()) != '\n' && getchar() != EOF);
    fgets(name, sizeof(name) ,stdin);
    do{
        printf("1.Add song to playlist\n");
        printf("2.Stop adding songs\n");
        printf("Enter your choice\n");
        scanf("%d",&ch);
        switch(ch){
            case 1: { printf("Select the song serial number \n");
                    scanf("%d",&sno);
                insertnode(&mylist, data[sno-1]);
                printf("The playlist: %s\n",name);
                printlist(&mylist);
                }
                break;
            case 2:printf("\nCreated playlist successfully\n");
            break;
            default:printf("Invalid choice\n");
              break;
            }
        }while(ch!=2);
        printf("Playing the playlist\n");
        printlist(&mylist);
        play(&mylist,data);
}
void pickasong(struct song data[9]){
    int i,sno,ch;
     for(i=0;i<9;i++){
        printf("%d. | %s\n",(i+1),data[i].songname);
    }
    printf("Pick a song\n");
    scanf("%d",&sno);
     mciSendString("close music", NULL, 0, NULL);
     mciSendString(data[sno-1].command,NULL,0,NULL);
        mciSendString("play music ",NULL,0,NULL);
        printf("Do you want to stop the song? Yes(1)|No(0)\n");
        scanf("%d",&ch);
        if(ch==1){
            mciSendString("close music",NULL,0,NULL);
            return ;
        }
        printf("Playback completed successfully.\n");
}
void queue(struct song data[9]){
    struct list mylist;
    initlist(&mylist);
    int i,sno,ch;
    printf("Available songs are: \n");
     for(i=0;i<9;i++){
        printf("%d. | %s\n",(i+1),data[i].songname);
    }
    printf("Pick a song\n");
    if(scanf("%d",&sno)!=1 || sno<1 || sno>9){
    printf("Invalid choice");
    return;
     }
    // Add initial song and set current pointer
    insertnode(&mylist, data[sno - 1]);
    struct node *current = mylist.head;

    // Start playing the initial song right away
    mciSendString("close music", NULL, 0, NULL);
    mciSendString(current->sdata.command, NULL, 0, NULL);
    mciSendString("play music", NULL, 0, NULL);

    do {
        printf("\nNow Playing: %s\n", current->sdata.songname);
        printf("1. Add song to play NEXT (Queue next)\n");
        printf("2. Go to next song in queue\n");
        printf("3. View current queue\n");
        printf("4. Stop playback & return to menu\n");
        printf("Enter choice: ");
        scanf("%d", &ch);

        switch (ch) {
            case 1:
                // Show song catalog
                printf("\n--- Song Catalog ---\n");
                for (i = 0; i < 9; i++) {
                    printf("%d. | %s\n", i + 1, data[i].songname);
                }
                printf("Enter serial no. to queue next: ");
                scanf("%d", &sno);

                if (sno >= 1 && sno <= 9) {
                    // Inserts immediately after the currently playing node
                    insertsongaftercurrent(&mylist, current, data[sno - 1]);
                    printf("Added '%s' right after current song.\n", data[sno - 1].songname);
                } else {
                    printf("Invalid song number!\n");
                }
                break;

            case 2:
                // Advance to next song in the queue
                if (current->next != NULL) {
                    current = current->next;
                    mciSendString("close music", NULL, 0, NULL);
                    mciSendString(current->sdata.command, NULL, 0, NULL);
                    mciSendString("play music", NULL, 0, NULL);
                } else {
                    printf("No more songs queued after this one!\n");
                }
                break;

            case 3:
                // Print remaining linked list queue
                printf("\n--- Upcoming Queue ---\n");
                printlist(&mylist);
                break;

            case 4:
                mciSendString("close music", NULL, 0, NULL);
                printf("Playback stopped.\n");
                break;

            default:printf("Invalid choice\n");
            }
        }while(ch!=2);
        mciSendString("close music",NULL,0,NULL);
        play(&mylist,data);
}