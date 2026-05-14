
#include <bits/stdc++.h>
#include <windows.h>
using namespace std;

int ROWS = 20;
int COLS = 40;
int SPEED = 20;

vector<vector<char>> grid;
vector<vector<int>> weights;

pair<int,int> startPos, endPos;

vector<pair<int,int>> directions =
{
    {1,0},{-1,0},{0,1},{0,-1}
};

void clearScreen()
{
    system("cls");
}

void wait()
{
    Sleep(SPEED);
}

bool isValid(int x, int y)
{
    return (x>=0 && y>=0 && x<ROWS && y<COLS);
}

bool isWall(int x, int y)
{
    return grid[x][y] == '#';
}

void printGrid()
{
    clearScreen();
    for(int i=0; i<ROWS; i++)
    {
        for(int j=0; j<COLS; j++)
        {
            cout << grid[i][j];
        }
        cout << endl;
    }
}


class GridManager
{
public:

    void initialize()
    {
        grid.assign(ROWS, vector<char>(COLS, '.'));
        weights.assign(ROWS, vector<int>(COLS, 1));
    }

    void setStartEnd()
    {
        while(true)
        {
            cout << "Enter Start (row col): ";
            cin >> startPos.first >> startPos.second;

            cout << "Enter End (row col): ";
            cin >> endPos.first >> endPos.second;

            if(isValid(startPos.first,startPos.second) &&
                    isValid(endPos.first,endPos.second) &&
                    startPos != endPos) break;

            cout << "Invalid input! Try again.\n";
        }

        grid[startPos.first][startPos.second] = 'S';
        grid[endPos.first][endPos.second] = 'E';
    }

    void addWalls()
    {
        int w;
        cout << "Number of walls: ";
        cin >> w;

        for(int i=0; i<w; i++)
        {
            int x,y;
            cin >> x >> y;

            if(isValid(x,y) && grid[x][y]=='.')
                grid[x][y] = '#';
        }
    }

    void randomWalls(int count)
    {
        while(count--)
        {
            int x = rand()%ROWS;
            int y = rand()%COLS;

            if(grid[x][y]=='.' &&
                    make_pair(x,y)!=startPos &&
                    make_pair(x,y)!=endPos)
                grid[x][y] = '#';
        }
    }

    void reset()
    {
        for(int i=0; i<ROWS; i++)
        {
            for(int j=0; j<COLS; j++)
            {
                if(grid[i][j]=='*' || grid[i][j]=='P')
                    grid[i][j]='.';
            }
        }
        grid[startPos.first][startPos.second]='S';
        grid[endPos.first][endPos.second]='E';
    }
};


bool reconstructPath(map<pair<int,int>, pair<int,int>> &parent)
{

    if(parent.find(endPos) == parent.end())
    {
        cout << "\n❌ No Path Found!\n";
        return false;
    }

    auto cur = endPos;

    while(cur != startPos)
    {
        cur = parent[cur];

        if(grid[cur.first][cur.second] != 'S')
            grid[cur.first][cur.second] = 'P';

        printGrid();
        wait();
    }
    return true;
}


void BFS()
{
    queue<pair<int,int>> q;
    set<pair<int,int>> visited;
    map<pair<int,int>, pair<int,int>> parent;

    q.push(startPos);
    visited.insert(startPos);

    while(!q.empty())
    {
        auto [x,y] = q.front();
        q.pop();

        if(make_pair(x,y) == endPos) break;

        for(auto [dx,dy] : directions)
        {
            int nx = x+dx, ny = y+dy;

            if(isValid(nx,ny) && !isWall(nx,ny) &&
                    !visited.count({nx,ny}))
            {

                visited.insert({nx,ny});
                parent[ {nx,ny}] = {x,y};
                q.push({nx,ny});

                if(grid[nx][ny] != 'E')
                    grid[nx][ny] = '*';

                printGrid();
                wait();
            }
        }
    }

    if(!reconstructPath(parent))
    {
        cout << "Press Enter...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
    }
}


bool DFSUtil(int x, int y,
             set<pair<int,int>> &visited,
             map<pair<int,int>, pair<int,int>> &parent)
{

    if(make_pair(x,y) == endPos)
        return true;

    visited.insert({x,y});

    for(auto [dx,dy] : directions)
    {
        int nx = x+dx, ny = y+dy;

        if(isValid(nx,ny) && !isWall(nx,ny) &&
                !visited.count({nx,ny}))
        {

            parent[ {nx,ny}] = {x,y};

            if(grid[nx][ny] != 'E')
                grid[nx][ny] = '*';

            printGrid();
            wait();

            if(DFSUtil(nx,ny,visited,parent))
                return true;
        }
    }
    return false;
}

void DFS()
{
    set<pair<int,int>> visited;
    map<pair<int,int>, pair<int,int>> parent;

    if(DFSUtil(startPos.first,startPos.second,visited,parent))
    {
        reconstructPath(parent);
    }
    else
    {
        cout << "\n❌ No Path Found!\n";
        cout << "Press Enter...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
    }
}


void Dijkstra()
{

    vector<vector<int>> dist(ROWS, vector<int>(COLS, INT_MAX));
    map<pair<int,int>, pair<int,int>> parent;
    set<pair<int,int>> visited;

    priority_queue<
    pair<int,pair<int,int>>,
         vector<pair<int,pair<int,int>>>,
         greater<>
         > pq;

    dist[startPos.first][startPos.second] = 0;
    pq.push({0,startPos});

    while(!pq.empty())
    {

        auto [d,pos] = pq.top();
        pq.pop();
        int x = pos.first, y = pos.second;

        if(visited.count(pos)) continue;
        visited.insert(pos);

        if(pos == endPos) break;

        for(auto [dx,dy] : directions)
        {
            int nx = x+dx, ny = y+dy;

            if(isValid(nx,ny) && !isWall(nx,ny))
            {

                int cost = weights[nx][ny];

                if(dist[x][y] + cost < dist[nx][ny])
                {

                    dist[nx][ny] = dist[x][y] + cost;
                    parent[ {nx,ny}] = {x,y};

                    pq.push({dist[nx][ny], {nx,ny}});

                    if(grid[nx][ny] != 'E')
                        grid[nx][ny] = '*';

                    printGrid();
                    wait();
                }
            }
        }
    }

    if(!reconstructPath(parent))
    {
        cout << "Press Enter...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
    }
}


void generateRandomMaze(int density)
{
    for(int i=0; i<ROWS; i++)
    {
        for(int j=0; j<COLS; j++)
        {
            if(rand()%100 < density && grid[i][j]=='.' &&
                    make_pair(i,j)!=startPos &&
                    make_pair(i,j)!=endPos)
                grid[i][j] = '#';
        }
    }
}

void setRandomWeights()
{
    for(int i=0; i<ROWS; i++)
    {
        for(int j=0; j<COLS; j++)
        {
            weights[i][j] = rand()%5 + 1;
        }
    }
}


void showMenu()
{
    cout << "\n1. BFS\n2. DFS\n3. Dijkstra\n4. Reset\n5. Random Maze\n6. Random Weight\n7. Speed\n8. Exit\n";
}


int main()
{

    srand(time(0));

    GridManager gm;
    gm.initialize();
    gm.setStartEnd();

    int opt;
    cout << "1. Manual Walls\n2. Random Walls\nChoice: ";
    cin >> opt;

    if(opt==1) gm.addWalls();
    else gm.randomWalls(100);

    int choice;

    while(true)
    {
        printGrid();
        showMenu();

        cin >> choice;

        if(choice==8) break;

        if(choice==1) BFS();
        else if(choice==2) DFS();
        else if(choice==3) Dijkstra();
        else if(choice==4)
        {
            gm.reset();
            printGrid();
        }
        else if(choice==5)
        {
            generateRandomMaze(30);
            printGrid();
        }
        else if(choice==6)
        {
            setRandomWeights();
            cout << "Weights Updated!\n";
        }
        else if(choice==7)
        {
            cout << "Enter speed: ";
            cin >> SPEED;
            cout << "Speed Updated!\n";
        }

        cout << "\nPress Enter...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
    }

    return 0;
}
