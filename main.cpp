#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>

class GameBoard {
public:
    static const int SIZE = 3;
    std::vector<std::vector<char>> board;

    GameBoard() {
        board = std::vector<std::vector<char>>(SIZE, std::vector<char>(SIZE, ' '));
    }

    void Display() const {
        for (int i = 0; i < SIZE; ++i) {
            for (int j = 0; j < SIZE; ++j) {
                std::cout << board[i][j];
                if (j < SIZE - 1) std::cout << "|";
            }
            std::cout << "\n";
            if (i < SIZE - 1) std::cout << "-----\n";
        }
    }

    bool IsFull() const {
        for (const auto& row : board) {
            for (char cell : row) {
                if (cell == ' ') return false;
            }
        }
        return true;
    }

    bool IsWinner(char player) const {
        // Check rows and columns
        for (int i = 0; i < SIZE; ++i) {
            if (board[i][0] == player && board[i][1] == player && board[i][2] == player) return true;
            if (board[0][i] == player && board[1][i] == player && board[2][i] == player) return true;
        }
        // Check diagonals
        if (board[0][0] == player && board[1][1] == player && board[2][2] == player) return true;
        if (board[0][2] == player && board[1][1] == player && board[2][0] == player) return true;
        return false;
    }
};

class IGameRepository {
public:
    virtual ~IGameRepository() = default;
    virtual void SaveMove(int row, int col, char player) = 0;
    virtual GameBoard GetBoard() = 0;
    virtual void ResetBoard() = 0;
};

class IGameService {
public:
    virtual ~IGameService() = default;
    virtual bool MakeMove(int row, int col, char player) = 0;
    virtual bool CheckWinner(char player) = 0;
    virtual bool IsBoardFull() = 0;
    virtual void ResetGame() = 0;
    virtual GameBoard GetBoard() = 0;
};

class GameRepository : public IGameRepository {
private:
    GameBoard _board;

public:
    void SaveMove(int row, int col, char player) override {
        _board.board[row][col] = player;
    }

    GameBoard GetBoard() override {
        return _board;
    }

    void ResetBoard() override {
        _board = GameBoard();
    }
};

class GameService : public IGameService {
private:
    std::shared_ptr<IGameRepository> _repository;

public:
    GameService(std::shared_ptr<IGameRepository> repository) : _repository(repository) {}

    bool MakeMove(int row, int col, char player) override {
        if (row < 0 || row >= GameBoard::SIZE || col < 0 || col >= GameBoard::SIZE) {
            return false; // Invalid move
        }
        if (_repository->GetBoard().board[row][col] != ' ') {
            return false; // Cell already occupied
        }
        _repository->SaveMove(row, col, player);
        return true;
    }

    bool CheckWinner(char player) override {
        return _repository->GetBoard().IsWinner(player);
    }

    bool IsBoardFull() override {
        return _repository->GetBoard().IsFull();
    }

    void ResetGame() override {
        _repository->ResetBoard();
    }

    GameBoard GetBoard() override {
        return _repository->GetBoard();
    }
};

class GameController {
private:
    std::shared_ptr<IGameService> _service;

public:
    GameController(std::shared_ptr<IGameService> service) : _service(service) {}

    bool MakeMove(int row, int col, char player) {
        return _service->MakeMove(row, col, player);
    }

    bool CheckWinner(char player) {
        return _service->CheckWinner(player);
    }

    bool IsBoardFull() {
        return _service->IsBoardFull();
    }

    void ResetGame() {
        _service->ResetGame();
    }

    GameBoard GetBoard() {
        return _service->GetBoard();
    }
};

class GamePresentation {
private:
    std::shared_ptr<GameController> _controller;
    char _currentPlayer;

public:
    GamePresentation(std::shared_ptr<GameController> controller) : _controller(controller), _currentPlayer('X') {}

    void StartGame() {
        std::cout << "Welcome to Tic-Tac-Toe!\n";
        _controller->ResetGame();

        while (true) {
            DisplayBoard();
            GetPlayerMove();

            if (_controller->CheckWinner(_currentPlayer)) {
                DisplayBoard();
                std::cout << "Player " << _currentPlayer << " wins!\n";
                break;
            }

            if (_controller->IsBoardFull()) {
                DisplayBoard();
                std::cout << "It's a draw!\n";
                break;
            }

            SwitchPlayer();
        }
    }

private:
    void DisplayBoard() const {
        std::cout << "\n";
        _controller->GetBoard().Display();
        std::cout << "\n";
    }

    void GetPlayerMove() {
        int row, col;
        while (true) {
            std::cout << "Player " << _currentPlayer << ", enter your move (row and column, e.g., 1 2): ";
            std::cin >> row >> col;

            if (_controller->MakeMove(row, col, _currentPlayer)) {
                break;
            } else {
                std::cout << "Invalid move. Try again.\n";
            }
        }
    }

    void SwitchPlayer() {
        _currentPlayer = (_currentPlayer == 'X') ? 'O' : 'X';
    }
};

int main() {
    // Dependency Injection Setup
    auto repository = std::make_shared<GameRepository>();
    auto service = std::make_shared<GameService>(repository);
    auto controller = std::make_shared<GameController>(service);
    GamePresentation presentation(controller);

    presentation.StartGame();

    return 0;
}
