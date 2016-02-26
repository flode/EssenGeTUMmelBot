#include <iostream>

#include <tgbot/tgbot.h>
#include <unordered_map>


using namespace TgBot;

enum class Coming : uint8_t {
    yes,
    no,
    maybe
};

struct Vote {
    std::string firstName;
    Coming coming;

    Vote(const std::string &firstName, const Coming &coming) : firstName(firstName), coming(coming) { }
};

std::unordered_multimap<uint64_t, Vote> load() {
    std::unordered_multimap<uint64_t, Vote> multimap;
    std::ifstream ifstream("save.sav", std::ios_base::in);
    while (ifstream.good()) {
        std::pair<uint64_t, Vote> item{0,{"",Coming::no}};
        ifstream.read(reinterpret_cast<char*>(&item.first), sizeof(item.first));
        std::getline(ifstream, item.second.firstName, '\0');
        ifstream.read(reinterpret_cast<char*>(&item.second.coming), sizeof(item.second.coming));
        multimap.emplace(item);
        ifstream.peek();
    }
    return multimap;
}

void save(std::unordered_multimap<uint64_t, Vote> &multimap) {
    std::ofstream of("save.sav", std::ios_base::out);
    for (auto &item : multimap) {
        of.write(reinterpret_cast<const char*>(&item.first), sizeof(item.first));
        of.write(item.second.firstName.c_str(), item.second.firstName.length());
        of.write("\0", 1);
        of.write(reinterpret_cast<const char*>(&item.second.coming), sizeof(item.second.coming));
    }
    std::cerr << "Finished saving!" << std::endl;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <telegram bot token>" << std::endl;
        return 0;
    }
    Bot bot(argv[1]);
    bot.getEvents().onCommand("start", [&bot](Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Hi!");
    });
    std::unordered_multimap<uint64_t, Vote> votes = load();


    bot.getEvents().onCommand("help", [&bot](Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id,
                                 "Hi " + message->from->firstName + ", I help organizing lunch times :) \n" +
                                 "Help: /help \n" +
                                 "Ask for Lunch time: /asklunch \n" +
                                 "Stop poll and show results: /results \n"
        );
    });
    bot.getEvents().onCommand("asklunch", [&bot, &votes](Message::Ptr message) {
        ReplyKeyboardMarkup::Ptr keyboardMarkup(new ReplyKeyboardMarkup);
        keyboardMarkup->keyboard = {{"yes"},
                                   {"no"}};
        keyboardMarkup->resizeKeyboard = true;
        keyboardMarkup->oneTimeKeyboard = true;
        keyboardMarkup->selective = false;
        bot.getApi().sendMessage(message->chat->id, "Coming to lunch today?", false, 0, keyboardMarkup);

        votes.clear();
    });
    bot.getEvents().onCommand("results", [&bot, &votes](Message::Ptr message) {
        ReplyKeyboardHide::Ptr hideKeyboard(new ReplyKeyboardHide);
        std::string results = "Coming to Lunch:\n";
        for (auto &vote : votes) {
            if (vote.second.coming == Coming::yes) {
                results += vote.second.firstName + "\n";
            }
        }
        results += "\nNot coming:\n";
        for (auto &vote : votes) {
            if (vote.second.coming == Coming::no) {
                results += vote.second.firstName + "\n";
            }
        }
        bot.getApi().sendMessage(message->chat->id, results, false, 0, hideKeyboard);
    });
    bot.getEvents().onCommand("add", [&bot, &votes] (Message::Ptr message) {
        if (message->from->id == 14480696) {
            return;
        }
        std::istringstream iss(message->text);
        std::vector<std::string> tokens{std::istream_iterator<std::string>{iss},
                                        std::istream_iterator<std::string>{}};
        if (tokens.size() != 3) {
            return;
        }

        if (tokens[2] == "yes") {
            votes.emplace(std::piecewise_construct,
                          std::forward_as_tuple(0),
                          std::forward_as_tuple(tokens[1], Coming::yes));
        } else {
            votes.emplace(std::piecewise_construct,
                          std::forward_as_tuple(0),
                          std::forward_as_tuple(tokens[1], Coming::no));
        }
        save(votes);
    });
    bot.getEvents().onNonCommandMessage([&votes] (Message::Ptr message) {
        if (message->text == "yes") {
            votes.erase(message->from->id);
            votes.emplace(std::piecewise_construct,
                          std::forward_as_tuple(message->from->id),
                          std::forward_as_tuple(message->from->firstName, Coming::yes));
        } else if (message->text == "no") {
            votes.erase(message->from->id);
            votes.emplace(std::piecewise_construct,
                          std::forward_as_tuple(message->from->id),
                          std::forward_as_tuple(message->from->firstName, Coming::no));
        }
        save(votes);
    });

    try {
        printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
        TgLongPoll longPoll(bot);
        while (true) {
            printf("Long poll started\n");
            longPoll.start();
        }
    } catch (TgException& e) {
        printf("error: %s\n", e.what());
    }
    return 0;
}
