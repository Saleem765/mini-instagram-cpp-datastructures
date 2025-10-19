#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <ctime>
#include<conio.h>
#include <Windows.h>
using namespace std;

// ------------------------ Node Class for Linked List ------------------------
class Node {
public:
    string data;
    Node* next;

    Node(string data) : data(data), next(nullptr) {}
};

// ------------------------ Stack Implementation ------------------------
class Stack {
private:
    Node* topNode;

public:
    Stack() : topNode(nullptr) {}

    void push(const string& data) {
        Node* newNode = new Node(data);
        newNode->next = topNode;
        topNode = newNode;
    }

    string pop() {
        if (isEmpty()) {
            throw runtime_error("Stack is empty.");
        }
        Node* temp = topNode;
        string data = temp->data;
        topNode = topNode->next;
        delete temp;
        return data;
    }

    bool isEmpty() {
        return topNode == nullptr;
    }
};

// ------------------------ Queue Implementation ------------------------
class Queue {
private:
    Node* frontNode;
    Node* rearNode;

public:
    Queue() : frontNode(nullptr), rearNode(nullptr) {}

    void enqueue(const string& data) {
        Node* newNode = new Node(data);
        if (rearNode) {
            rearNode->next = newNode;
        }
        rearNode = newNode;
        if (!frontNode) {
            frontNode = rearNode;
        }
    }

    string dequeue() {
        if (isEmpty()) {
            throw runtime_error("Queue is empty.");
        }
        Node* temp = frontNode;
        string data = temp->data;
        frontNode = frontNode->next;
        if (!frontNode) {
            rearNode = nullptr;
        }
        delete temp;
        return data;
    }

    bool isEmpty() {
        return frontNode == nullptr;
    }
};

// ------------------------ Post Class ------------------------
class Post {
public:
    string dateTime;
    string content;

    Post(const string& content) {
        time_t now = time(0);
        char buffer[26];
        ctime_s(buffer, sizeof(buffer), &now);
        dateTime = string(buffer);
        this->content = content;
    }
};

// ------------------------ User Class ------------------------

class User {
public:
    static int userIdCounter;  // Static counter to auto-generate user IDs
    int userId;                // Unique ID for each user
    string username;
    string password;
    string city;
    Stack posts;
    Queue friendRequests;
    vector<string> friends;
    queue<string> messages;

    User* next;

    User(const string& username, const string& password, const string& city)
        : username(username), password(password), city(city), next(nullptr) {
        userId = ++userIdCounter;  // Assign auto-generated user ID
    }

    void addPost(const string& content) {
        Post newPost(content);
        posts.push(newPost.content + " (" + newPost.dateTime + ")");
        cout << "Post created successfully.\n";
    }


    void viewPosts() {
        if (posts.isEmpty()) {
            cout << "No posts to display.\n";
            return;
        }
        cout << "\n--- Your Posts ---\n";

        Stack tempStack;
        Stack temp2;
        string temp;
        // Reverse the original stack using a temporary stack
        while (!posts.isEmpty()) {
            temp = posts.pop();
            tempStack.push(temp);
            temp2.push(temp);
        }

        // Display the posts from the reversed stack
        while (!tempStack.isEmpty()) {
            cout << tempStack.pop() << "\n";
        }

        // Restore the original stack
        while (!temp2.isEmpty()) {
            posts.push(temp2.pop());
        }
    }


    void sendFriendRequest(User* recipient) {
        if (!recipient) {
            cout << "User not found.\n";
            return;
        }
        recipient->friendRequests.enqueue(this->username);
        cout << "Friend request sent to " << recipient->username << ".\n";
    }

    void viewFriendRequests() {
        if (friendRequests.isEmpty()) {
            cout << "No friend requests.\n";
            return;
        }
        cout << "\n--- Friend Requests ---\n";
        Queue tempQueue = friendRequests;
        while (!tempQueue.isEmpty()) {
            cout << "Request from: " << tempQueue.dequeue() << "\n";
        }
    }

    void approveFriendRequest(const string& friendUsername) {
        Queue tempQueue;
        bool found = false;

        while (!friendRequests.isEmpty()) {
            string currentRequest = friendRequests.dequeue();
            if (currentRequest == friendUsername) {
                friends.push_back(friendUsername);
                cout << "You are now friends with " << friendUsername << ".\n";
                found = true;
            }
            else {
                tempQueue.enqueue(currentRequest);
            }
        }

        // Restore unprocessed friend requests
        while (!tempQueue.isEmpty()) {
            friendRequests.enqueue(tempQueue.dequeue());
        }

        if (!found) {
            cout << "No friend request found from " << friendUsername << ".\n";
        }
    }

    void sendMessage(const string& recipientUsername, const string& message, User* recipient) {
        if (!recipient) {
            cout << "User not found.\n";
            return;
        }
        if (find(friends.begin(), friends.end(), recipientUsername) == friends.end()) {
            cout << "You can only send messages to friends.\n";
            return;
        }
        recipient->messages.push(username + ": " + message);
        cout << "Message sent to " << recipientUsername << ".\n";
    }

    void viewMessages() {
        if (messages.empty()) {
            cout << "No messages to display.\n";
            return;
        }
        cout << "\n--- Your Messages ---\n";
        while (!messages.empty()) {
            cout << messages.front() << "\n";
            messages.pop();
        }
    }
};

int User::userIdCounter = 0;  // Initialize static user ID counter


// ------------------------ Instagram System ------------------------

class Instagram {
private:
    User* users;        // Linked list of users
    User* loggedInUser;
    static vector<vector<int>> graph;  // Dynamic 2D array for user relations

public:
    Instagram() : users(nullptr), loggedInUser(nullptr) {}

    void signup(const string& username, const string& password, const string& city) {
        if (searchUser(username)) {
            cout << "Error: Username already exists.\n";
            return;
        }
        if (password.length() < 2) {
            cout << "Error: Password must be at least 8 characters long.\n";
            return;
        }
        User* newUser = new User(username, password, city);
        newUser->next = users;
        users = newUser;

        // Resize the graph array (add new row and column for the new user)
        int newSize = newUser->userId;  // new user ID is equal to the number of users
        graph.resize(newSize);
        for (auto& row : graph) {
            row.resize(newSize, 0);  // Initialize new columns with 0
        }

        cout << "Signup successful!\n";
        cout << endl << endl;
        for (int i = 0; i < newSize; i++)
        {
            for (int j = 0; j < newSize; j++)
            {
                cout << graph[i][j] << "\t";
            }
            cout << endl;
        }
    }

    void login(const string& username, const string& password) {
        User* user = searchUser(username);
        if (user && user->password == password) {
            loggedInUser = user;
            cout << "Login successful!\n";
        }
        else {
            cout << "Invalid username or password.\n";
        }
    }

    void logout() {
        if (loggedInUser) {
            loggedInUser = nullptr;
            cout << "Logout successful!\n";
        }
        else {
            cout << "No user is logged in.\n";
        }
    }
    void createPost(const string& content) {
        if (!loggedInUser) {
            cout << "Please log in first.\n";
            return;
        }
        loggedInUser->addPost(content);
    }

    void viewPosts() {
        if (!loggedInUser) {
            cout << "Please log in first.\n";
            return;
        }
        loggedInUser->viewPosts();
    }

    void sendFriendRequest(const string& recipientUsername) {
        if (!loggedInUser) {
            cout << "Please log in first.\n";
            return;
        }
        User* recipient = searchUser(recipientUsername);
        if (recipient) {
            loggedInUser->sendFriendRequest(recipient);
            string k = "Request has Send to you by " + loggedInUser->username;
            sendMessage(recipientUsername, k);
            graph[loggedInUser->userId - 1][recipient->userId - 1] = 2;  // Set request state (2)
        }
    }

    void approveFriendRequest(const string& friendUsername) {
        if (!loggedInUser) {
            cout << "Please log in first.\n";
            return;
        }
        User* recipient = searchUser(friendUsername);
        if (recipient) {
            loggedInUser->approveFriendRequest(friendUsername);
            string k = "Request had excepted by " + loggedInUser->username;
            sendMessage(friendUsername, k);
            graph[loggedInUser->userId - 1][recipient->userId - 1] = 1;  // Set friends state (1)
            graph[recipient->userId - 1][loggedInUser->userId - 1] = 1;  // Set friends state (1) for both users
        }
    }
    void sendMessage(const string& recipientUsername, const string& message) {
        if (!loggedInUser) {
            cout << "Please log in first.\n";
            return;
        }
        User* recipient = searchUser(recipientUsername);
        loggedInUser->sendMessage(recipientUsername, message, recipient);
    }

    void viewMessages() {
        if (!loggedInUser) {
            cout << "Please log in first.\n";
            return;
        }
        loggedInUser->viewMessages();
    }
    void friendSuggestion() {
        if (!loggedInUser) {
            cout << "Please log in first.\n";
            return;
        }

        int loggedInUserId = loggedInUser->userId - 1;  // Index of logged-in user in the graph
        cout << "\n--- Friend Suggestions ---\n";

        // Iterate through the friends of the logged-in user
        for (int i = 0; i < graph.size(); ++i) {
            if (graph[loggedInUserId][i] == 1) {  // If the user is a friend
                // Look at the friends of this friend
                for (int j = 0; j < graph.size(); ++j) {
                    if (graph[i][j] == 1 && graph[loggedInUserId][j] == 0 && j != loggedInUserId) {
                        // If this is a friend of a friend and not already a friend of logged-in user
                        User* suggestedUser = searchUserById(j + 1);  // Assuming user IDs are 1-based
                        if (suggestedUser) {
                            cout << suggestedUser->username << "\n";  // Display the friend suggestion
                        }
                    }
                }
            }
        }
    }

    void stalking(const string& targetUsername) {
        if (!loggedInUser) {
            cout << "Please log in first.\n";
            return;
        }

        User* targetUser = searchUser(targetUsername);
        if (!targetUser) {
            cout << "User not found.\n";
            return;
        }

        int loggedInUserId = loggedInUser->userId - 1;  // Index of logged-in user
        int targetUserId = targetUser->userId - 1;  // Index of target user

        // Check if the logged-in user and target user are friends (using the graph)
        if (graph[loggedInUserId][targetUserId] == 1) {
            // If they are friends, show the posts of the target user
            cout << "\n--- " << targetUsername << "'s Posts ---\n";
            targetUser->viewPosts();
        }
        else {
            cout << "You can only view posts of friends.\n";
        }
    }
    void interpretGraph(const string& targetUsername) {
        if (!loggedInUser || loggedInUser->username != "admin") {
            cout << "Only admin can perform this action.\n";
            return;
        }

        User* targetUser = searchUser(targetUsername);
        if (!targetUser) {
            cout << "User not found.\n";
            return;
        }

        int targetUserId = targetUser->userId - 1;  // Index of target user in the graph

        // Describe friends
        cout << "\n--- Friends of " << targetUsername << " ---\n";
        bool foundFriend = false;
        for (int i = 0; i < graph.size(); ++i) {
            if (graph[targetUserId][i] == 1) {  // Friend relationship
                User* friendUser = searchUserById(i + 1);  // Get friend by ID (1-based)
                if (friendUser) {
                    cout << friendUser->username << "\n";
                    foundFriend = true;
                }
            }
        }
        if (!foundFriend) {
            cout << "No friends found.\n";
        }

        // Describe friend requests sent by the target user
        cout << "\n--- Friend Requests Sent by " << targetUsername << " ---\n";
        foundFriend = false;
        for (int i = 0; i < graph.size(); ++i) {
            if (graph[i][targetUserId] == 2) {  // Sent request (2 indicates sent request)
                User* recipientUser = searchUserById(i + 1);
                if (recipientUser) {
                    cout << recipientUser->username << "\n";
                    foundFriend = true;
                }
            }
        }
        if (!foundFriend) {
            cout << "No friend requests sent.\n";
        }

        // Describe friend requests received by the target user
        cout << "\n--- Friend Requests Received by " << targetUsername << " ---\n";
        foundFriend = false;
        for (int i = 0; i < graph.size(); ++i) {
            if (graph[targetUserId][i] == 2) {  // Received request (2 indicates received request)
                User* requesterUser = searchUserById(i + 1);
                if (requesterUser) {
                    cout << requesterUser->username << "\n";
                    foundFriend = true;
                }
            }
        }
        if (!foundFriend) {
            cout << "No friend requests received.\n";
        }
    }

private:
    User* searchUser(const string& username) {
        User* current = users;
        while (current) {
            if (current->username == username) {
                return current;
            }
            current = current->next;
        }
        return nullptr;
    }
    User* searchUserById(int userId) {
        User* current = users;
        while (current) {
            if (current->userId == userId) {
                return current;
            }
            current = current->next;
        }
        return nullptr;  // Return nullptr if the user is not found
    }


};

// Initialize the static member outside the class definition
vector<vector<int>> Instagram::graph;


// ------------------------ Main Function ------------------------
int main() {
    Instagram instagram;
    int choice;
    string k;
    instagram.signup("admin", "12346578", "FSD");
    do {
        cout << "\n--- Mini Instagram Menu ---\n";
        cout << "1. Signup\n";
        cout << "2. Login\n";
        cout << "3. Logout\n";
        cout << "4. Create Post\n";
        cout << "5. View Posts\n";
        cout << "6. Send Friend Request\n";
        cout << "7. Approve Friend Request\n";
        cout << "8. Send Message\n";
        cout << "9. View Messages\n";
        cout << "10. Exit\n";
        cout << "11. See Suggestions\n";
        cout << "12. Stalking\n";
        cout << "13.Monitor Activites of user (only admin allowed)";
        cout << "Enter your choice: ";
        cin >> choice;

        string username, password, city, content, message;

        switch (choice) {
        case 1:
            while (1)
            {
                cout << "Enter username: ";
                cin >> username;
                if (username != "admin")
                    break;
                else
                    cout << "Invalid Name" << endl;
            }
            cout << "Enter password (min 8 characters): ";
            cin >> password;
            cout << "Enter city: ";
            cin >> city;
            instagram.signup(username, password, city);
            break;

        case 2:
            cout << "Enter username: ";
            cin >> username;
            cout << "Enter password: ";
            cin >> password;
            instagram.login(username, password);
            break;

        case 3:
            instagram.logout();
            break;

        case 4:
            cout << "Enter content for your post: ";
            cin.ignore();
            getline(cin, content);
            instagram.createPost(content);
            break;

        case 5:
            instagram.viewPosts();
            break;

        case 6:
            cout << "Enter the username to send a friend request: ";
            cin >> username;
            instagram.sendFriendRequest(username);
            break;

        case 7:
            cout << "Enter the username whose friend request you want to approve: ";
            cin >> username;
            instagram.approveFriendRequest(username);
            break;

        case 8:
            cout << "Enter the recipient's username: ";
            cin >> username;
            cout << "Enter your message: ";
            cin.ignore();
            getline(cin, message);
            instagram.sendMessage(username, message);
            break;

        case 9:
            instagram.viewMessages();
            break;

        case 10:
            cout << "Exiting... Goodbye!\n";
            break;
        case 11:
            cout << "You can make friendship with = ";
            instagram.friendSuggestion();
            break;
        case 12:
            cout << "To Stalking Your Friend\nEnter his username = ";
            cin >> k;
            instagram.stalking(k);
            break;
        case 13:
            cout << "Enter whom you want to Monitor = ";
            cin >> k;
            instagram.interpretGraph(k);

        default:
            cout << "Invalid choice. Please try again.\n";
            break;
        }
        Sleep(3000);
        system("cls");
    } while (choice != 10);
    system("pause");
    return 0;
}