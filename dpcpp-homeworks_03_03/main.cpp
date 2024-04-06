#include <iostream>
#include <memory>
#include <fstream>
#include <string>



enum class Type : int32_t {
    Warning,
    Error,
    FatalError,
    UnknownMessage,
};

class LogMessage {
public:
    virtual ~LogMessage() = default;
    virtual Type type() const=0;
    virtual const std::string message() const=0;
};

//предупреждение
class Warning :public LogMessage {
public:
    Type type() const override {
        return Type::Warning;
    }
    const std::string message() const override {
        return "Warning message";
    }
};

//ошибка
class Error :public LogMessage {
public:
    Type type()const override {
        return Type::Error;
    }
    const std::string message() const override {
        return "Error message!";
    }
};

//фатальная ошибка
class FatalError :public LogMessage {
public:
    Type type() const override {
        return Type::FatalError;
    }
    const std::string message() const override {
        return "FatalError message!";
    }
};
//неизвестное сообщение
class UnknownMessage :public LogMessage {
public:
    Type type()const override {
        return Type::UnknownMessage;
    }
    const std::string message() const override {
        return "UNKNOWN message!";
    }
};


//обработчик
class Handler {
public:
    virtual ~Handler() = default;
    explicit Handler(std::unique_ptr<Handler> next) : next_(std::move(next)){}
    void reciveMessage(const LogMessage& message) {
        if (toHandle(message)) {
            return;
        }
        else {
        }
        if (!next_) {
            std::cout << "Unable to handle this message!" << std::endl;
        }
        next_->reciveMessage(message);
    };
private:
    virtual bool toHandle(const LogMessage& message) =0;
    std::unique_ptr<Handler> next_;
};

//fatal error hanlder
class FatalErHandler :public Handler {
    Type type_ = Type::FatalError;  
    bool toHandle(const LogMessage& message) override {
        if (message.type() == type_) {
            throw std::exception(message.message().c_str());
            return true;
        }
        return false;
    }
public:
    using Handler::Handler;
};

//обычная ошибка
class ErrorHandler :public Handler {
    Type type_ = Type::Error;
    bool toHandle(const LogMessage& message) override {
        if (message.type() == type_) {
            std::ofstream out;
            out.open("..\\log.txt");
            if (out.is_open()) {
                out << message.message() << std::endl;
            }
            else {
                throw std::runtime_error("Unable to open log.txt");
            }
            return true;
        }
        return false;
    }
public:
    using Handler::Handler;
};

//предупреждение
class WarningHandler : public Handler {
    Type type_ = Type::Warning;
    bool toHandle(const LogMessage& message) override {
        if (message.type() == type_) {
            std::cout << message.message() << std::endl;
            return true;
        }
        return false;
    }
public:
    using Handler::Handler;
};

class UnknownHandler : public Handler {
    Type type_ = Type::UnknownMessage;
    bool toHandle(const LogMessage& message) override {
        if (message.type() == type_) {
            throw std::exception("Unable to handle this unknown error...");
        }
        return true;
    }
public:
    using Handler::Handler;
};

void print(LogMessage& a) {
    std::cout << a.message();
    //std::cout << "test?";
}

int main() {
    try {       
        auto unknown_h_ptr = std::make_unique<UnknownHandler>(nullptr);
        auto warn_h_ptr = std::make_unique<WarningHandler>(std::move(unknown_h_ptr));
        auto error_h_ptr = std::make_unique<ErrorHandler>(std::move(warn_h_ptr));
        auto fataler_h_ptr = std::make_unique<FatalErHandler>(std::move(error_h_ptr));

        //fataler_h_ptr->reciveMessage(UnknownMessage());
        fataler_h_ptr->reciveMessage(Warning());
        fataler_h_ptr->reciveMessage(Error());
        fataler_h_ptr->reciveMessage(FatalError());
     
     ///*   std::unique_ptr<WarningHandler> pmp = std::make_unique<WarningHandler>(nullptr);
     //   pmp->reciveMessage(Warning());*/
        //FatalError man;
        ////std::cout << man.message() << std::endl;
        //print(man);
    }
    catch (std::exception& er) {
        std::cout << er.what() << std::endl;
    }

    return 0;
}