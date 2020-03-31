#include <string>
#include <iostream>
#include <ctime>
#include <vector>
#include <array>
#include <fstream>

using namespace std;


class Tache{ //classe représentant une tache de la todo-list

    
    private: //variable privée, accessible via les fonctions get et set
    string titre;
    string description;
    string dateCreation;
    string dateCloture;
    string status; // open, in progress, closed
    int avancement; //en pourcentage
    string priorite; // high, low, normal
    vector<string> commentaires;
    int tacheMere;

    public:
    vector<Tache> ssTaches;
    Tache(string tTitre, string tDescription, string tPriorite, int tMere = -1){ // le constructeur initialise la tache avec les valeurs entrées et écrit la date de création
        titre = tTitre;
        tacheMere = tMere;
        description = tDescription;
        if(tPriorite == "normal"){
            priorite = "normal";
        }
        if(tPriorite == "high"){
            priorite = "high";
        }
        if(tPriorite == "low"){
            priorite = "low";
        }
        time_t now = time(0);
        dateCreation = ctime(&now);
        status = "open";
        avancement = 0;
    }

   //fonctions pour interagir avec les attributs de la tâche
    void addcommentaire(string commentaire){
        commentaires.push_back(commentaire);
    }

    void setAvancement(int avance){
        avancement = avance;
    }

    void setStatus(string stat){
        status = stat;
    }

    void setDateCreation(string dateCrea){
        dateCreation = dateCrea;
    }

    void setDateCloture(string dateClo){
        dateCloture = dateClo;
    }

    string getStatus(){
        return status;
    }

    string getTitre(){
        return titre;
    }

    string getDateCreation(){
        return dateCreation;
    }

    string getDateCloture(){
        return dateCloture;
    }

    vector<string> getCommentaires(){
        return commentaires;
    }

    string getDescription(){
        return description;
    }

    string getPriorite(){
        return priorite;
    }

    void addSousTache(Tache ssTache){
        ssTaches.push_back(ssTache);
    }


    void close(){ //fermeture d'une tâche, c'est à dire modification de son status, enregistrement de l'heure de cloture ainsi que cloture de ses taches filles si elles existent
        status = "closed";
        time_t now = time(0);
        dateCloture = ctime(&now);
        for(int i = 0; i < ssTaches.size(); i++){
            ssTaches[i].close();
        }
    }


};

//liste de Taches, c'est la "todo-list", variable globale
vector<Tache> mainTaches;

Tache* selectTache(string strID){ //fonction permettant de retourner un pointeur vers une tâche à partir de son id sous forme de string
    int id;
    try{ //on essaie de le transformer en int et si la variable ne correspond pas à une valeur d'id existente, on gère l'erreur
        id = stoi(strID);
    }
    catch(const invalid_argument e){
        cout << "invalid id" << endl;
        return nullptr;
    }
    if(id < 0 || id > mainTaches.size()){
        cout << "invalid id" << endl;
        return nullptr;
    }
    return &mainTaches[id];
}

Tache* selectSsTache(string strID, string motherStrID){ //fonction permettant de retourner un pointeur vers une sous-tâche à partir de son id et l'ID de la tâche mère sous forme de string 
        int id;
        int motherid;
        try{
            id = stoi(strID);
            motherid = stoi(motherStrID);
        }
        catch(const invalid_argument e){
            return nullptr;
        }
        if(motherid < 0 || motherid > mainTaches.size() || id < 0 || id > mainTaches[motherid].ssTaches.size()){
            cout << "invalid id" << endl;
            return nullptr;
        }

        return &mainTaches[motherid].ssTaches[id];
}

void save(){ //fonction sauvegardant les tâches dans un fichier .txt
    ofstream flux("C:/Users/Jonathan/Workspace/ProjetCpp/save.txt");
    if(flux){ //on ouvre le fichier et exécute ce code si il est ouvert correctement, sinon on gère l'erreur dans le "else"


        for(int i = 0; i < mainTaches.size(); i++){ //pour chaque tache on écrit "{" puis sur chaque ligne ses arguments sous la forme <nom de l'argument> <argument> puis }
            flux << "{" << endl;
            flux << "title " << mainTaches[i].getTitre() << endl;
            flux << "description " << mainTaches[i].getDescription() << endl;
            string status = mainTaches[i].getStatus();
            flux << "status " << status << endl;
            flux << "creation " << mainTaches[i].getDateCreation() << endl;
            if(status == "closed"){
                flux << "cloture " << mainTaches[i].getDateCloture() << endl;
            }
            flux << "priorite " << mainTaches[i].getPriorite() << endl;
            vector<string> comments = mainTaches[i].getCommentaires();
            for(int j = 0; j < comments.size(); j++){
                flux << "comment " << comments[j] << endl;
            }
            flux << "}" << endl;

            vector<Tache> ssTaches = mainTaches[i].ssTaches; //on écrit ensuite ses sous-tâches selon le même principe sauf qu'on rajoute une ligne "child" pour indiquer que c'est une sous-tâche de la précédente tâche principale
            for(int j = 0; j < ssTaches.size(); j++){
                flux << "{" << endl;
                flux << "child" << endl;
                flux << "title " << ssTaches[i].getTitre() << endl;
                flux << "description " << ssTaches[i].getDescription() << endl;
                string status = ssTaches[i].getStatus();
                flux << "status " << status << endl;
                flux << "creation " << ssTaches[i].getDateCreation() << endl;
                if(status == "closed"){
                    flux << "cloture " << ssTaches[i].getDateCloture() << endl;
                }
                flux << "priorite " << ssTaches[i].getPriorite() << endl;
                vector<string> comments = ssTaches[i].getCommentaires();
                for(int k = 0; k < comments.size(); k++){
                   flux << "comment " << comments[k] << endl;
                }
                flux << "}" << endl;
            }
        }
    } else {
        cout << "erreur d'ouverture du fichier" << endl;
    }
}

void process(string input){ //fonction qui prend la ligne entrée, récupère la commande et les arguments puis éxecute le code correspondant à la commande

    //on sépare le mot clé de la commande du reste, puis on découpe le reste en liste de couple mot-clé de l'option et contenu de l'option
    string command = input.substr(0, input.find(" "));
    string rest = input.substr(min(input.find(" "),input.size()), input.size());
    vector<array<string,2> > options;
    size_t optPos = rest.find(" --");
    string opt;
    size_t sep;
    while(optPos != string::npos){
        rest = rest.substr(optPos + 3, string::npos);
        optPos = rest.find(" --");
        opt = rest.substr(0,optPos);
        sep = rest.find(" ");
        options.push_back({opt.substr(0,sep),opt.substr(sep + 1, string::npos)});
    }


    //pour chaque commande existante, on teste le mot-clé puis si c'est le bon on l'execute en prenant en compte les options
    if(command == "newtask"){ //commande qui créé une tâche

            //on rentre des valeurs par défaut des options
            int mere = -1;
            string titre = "";
            string description = "";
            string priorite = "normal";

            //pour chaque option on teste le mot-clé, si c'est le bon on rentre le contenu de l'option dans la variable correspondante
            for(int i = 0; i < options.size(); i++){
                if(options.at(i)[0] == "title"){
                    titre = options.at(i)[1];
                }
                if(options.at(i)[0] == "description"){
                    description = options.at(i)[1];
                }
                if(options.at(i)[0] == "priority"){
                    if(options.at(i)[1] == "high"){
                        priorite = "high";
                    }
                    if(options.at(i)[1] == "low"){
                        priorite = "low";
                    }
                }
                if(options.at(i)[0] == "mother"){
                    try{
                        mere = stoi(options.at(i)[1]);
                    }
                    catch(const invalid_argument e){
                        cout << "invalid mother id" << endl;
                        return;
                    }
                    if (mere >= mainTaches.size())
                    {
                        cout << "invalid mother id" << endl;
                        return;
                    }
                    
                }
            }
            //si la tâche n'a pas de mère on la met dans la liste mainTaches
            if(mere < 0){
                mainTaches.push_back(Tache(titre, description, priorite));
            } else { //si la tâche est une sous-tache on la mets dans la liste de sous-tâches de sa tâche mère
                mainTaches[mere].addSousTache(Tache(titre,description,priorite,mere));
            }
            cout << "Successfully created new task with title : " << titre << endl;
    }

    //la commande tasks permet d'afficher tous les tâches principales avec leurs id et leurs status
    if(command=="tasks"){
        cout << "Tasks :" << endl;
        for(int i = 0; i < mainTaches.size(); i++){
            cout << i << " : " << mainTaches[i].getTitre() << "  :  " << mainTaches[i].getStatus() << endl;
        }
    }

    //la commande subtasks a le même principe mais affiche les sous-tâches de la tâche donc on a fourni l'ID
    if(command=="subtasks"){
        string strID;
        for(int i = 0; i < options.size(); i++){
            if(options.at(i)[0] == "id"){
                strID = options[i][1];
            }
        }
        int id;
        try{
            id = stoi(strID);
        }
        catch(const invalid_argument e){
            cout << "invalid id" << endl;
            return;
        }
        if(id < 0 || id > mainTaches.size()){
            cout << "invalid id" << endl;
            return;
        }
        cout << "SubTasks :" << endl;
        for(int i = 0; i < mainTaches[id].ssTaches.size(); i++){
            cout << i << " : " << mainTaches[id].ssTaches[i].getTitre() << "  :  " << mainTaches[id].ssTaches[i].getStatus() << endl;
        }
    }
    
    //info permet d'afficher des informations sur la tâche selectionnée
    if(command=="info"){
        string strID;
        bool isChild = false;
        string motherstrID;

        //on récupère l'ID et éventuellement l'ID de la tâche mère sous forme de strings
        for(int i = 0; i < options.size(); i++){
            if(options.at(i)[0] == "id"){
                strID = options.at(i)[1];
            }
            if(options.at(i)[0] == "motherid"){
                isChild = true;
                motherstrID = options.at(i)[1];
            }
        }
        
        //On récupère un pointeur vers la tâche correspondante
        Tache* task;
        if(isChild){
            task = selectSsTache(strID, motherstrID);
        } else{
            task = selectTache(strID);
        }


        if(task){//Si il y a bien une tâche correspondante, on affiche ses arguments
            cout << (*task).getTitre() << " : " << (*task).getDescription() << endl;
            string status = (*task).getStatus();
            cout << "status : " << status << endl;
            cout << "cree le : " << (*task).getDateCreation() << endl;
            if(status == "closed"){
                cout << "clos le : " << (*task).getDateCloture() << endl;
            }
            cout << "priorite : " << (*task).getPriorite() << endl;
            vector<string> commentaires = (*task).getCommentaires();
            cout << "commentaires : " << endl;
            for(int i = 0; i < commentaires.size(); i++){
                cout << commentaires[i] << endl;
            }
        }

    }

    //close permet de clore une tâche, et fonctionne sur le même principe que info
    if(command=="close"){
        string strID;
        bool isChild = false;
        string motherstrID;
        
        //on récupère l'ID et éventuellement l'ID de la tâche mère sous forme de strings
        for(int i = 0; i < options.size(); i++){
            if(options.at(i)[0] == "id"){
                strID = options.at(i)[1];
            }
            if(options.at(i)[0] == "motherid"){
                isChild = true;
                motherstrID = options.at(i)[1];
            }
        }

        //On récupère un pointeur vers la tâche correspondante
        Tache* task;
        if(isChild){
            task = selectSsTache(strID, motherstrID);
        } else{
            task = selectTache(strID);
        }

        if(task){//Si on a bien trouvé une tâche, on la clot
            (*task).close();
        }

    }

    //quit permet de sauvegarder puis quitter le programme
    if(command=="quit"){
        save();
        exit(0);
    }

    //comment permet d'ajouter un commentaire à une tâche et fonctionne sur le même principe que info
    if(command=="comment"){
        string strID;
        bool isChild = false;
        string motherstrID;
        string comment;

        
        //on récupère l'ID et éventuellement l'ID de la tâche mère sous forme de strings
        for(int i = 0; i < options.size(); i++){
            if(options.at(i)[0] == "id"){
                strID = options.at(i)[1];
            }
            if(options.at(i)[0] == "motherid"){
                isChild = true;
                motherstrID = options.at(i)[1];
            }
            if(options.at(i)[0] == "comment"){
                comment = options.at(i)[1];
            }
        }

        //On récupère un pointeur vers la tâche correspondante
        Tache* task;
        if(isChild){
            task = selectSsTache(strID, motherstrID);
        } else{
            task = selectTache(strID);
        }

        if(task){//Si on a bien trouvé la tâche, on ajoute le commentaire
            (*task).addcommentaire(comment);
        }
    }
}

void load(){//fonction éxécutée au démarrage du programme qui permet de charger les données sauvegardées

    //Ouverture du fichier
    ifstream flux("C:/Users/Jonathan/Workspace/ProjetCpp/save.txt");

    if(flux){//Si il est bien ouvert on exécute ce code, sinon on gère l'erreur dans le "else"
        string ligne;
        string titre;
        string description;
        vector<string> commentaires;
        string status;
        string dateCreation;
        string dateCloture;
        string priorite;
        bool isChild = false;
        while(getline(flux,ligne)){//On lit ligne par ligne

            //Pour chaque ligne on sépare le mot-clé et le contenu 
            string command = ligne.substr(0, ligne.find(" "));
            string rest = ligne.substr(min(ligne.find(" ") + 1,ligne.size()), ligne.size());

            //selon le mot-clé on met le contenu dans une variable
            if(command=="{"){ //si le mot-clé est "{", on efface les variables qui doivent l'être car cela signifie qu'on entre dans une nouvelle tâche
                commentaires.clear();
                isChild = false;
            }
            if(command=="child"){
                isChild = true;
            }
            if(command=="title"){
                titre = rest;
            }
            if(command=="description"){
                description = rest;
            }
            if(command=="status"){
                status = rest;
            }
            if(command=="creation"){
                dateCreation=rest;
            }
            if(command=="cloture"){
                dateCloture=rest;
            }
            if(command=="priorite"){
                priorite = rest;
            }
            if(command=="comment"){
                commentaires.push_back(rest);
            }
            if(command=="}"){//ce mot-clé signifie la fin de la tâche, on instancie donc l'objet avec les variables enregistrées
                Tache task(titre,description,priorite);
                task.setStatus(status);
                task.setDateCreation(dateCreation);
                if(status=="closed"){
                    task.setDateCloture(dateCloture);
                }
                for(int i = 0; i < commentaires.size(); i++){
                    task.addcommentaire(commentaires[i]);
                }

                //puis on l'ajoute à mainTaches, ou alors aux sousTaches de sa tache mère
                if(isChild){
                    mainTaches[mainTaches.size() - 1].addSousTache(task);
                } else {
                    mainTaches.push_back(task);
                }
            }
        }
    } else {
        cout << "erreur d'ouverture du fichier de sauvegarde" << endl;
        return;
    }
}

int main(){
    load();
    cout << ">";
    while(1){//Cette boucle demande une commande puis l'execute
        string command;
        getline(cin,command);
        process(command);
        cout << ">";
    }
}
