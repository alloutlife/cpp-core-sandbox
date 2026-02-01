// This playground demonstrates how to organise a recursion without recursive
// function calls.

// Consider solving a bit tricky leetcode problem
// [url](https://leetcode.com/problems/remove-invalid-parentheses/)

// The approach to finding a solution for this task is:
//
// 1. find minimum deletes required to make the expression valid
// 2. brute force the string by removing each parenthesis and checking if
//    modified string is good

#include <cassert>
#include <deque>
#include <stack>
#include <string>
#include <unordered_set>
#include <vector>

// Ideas for optimization:
//
// - We can estimate a minimum number of deletes required to make the string
//   valid, so we shouldn't need to process those strings that contain larger
//   number of deletes.
// - different intermediate deletes may give the same result. Use memoization in
//   order to skip processing of known strings.
// - there is no need to test removals of non-parentheses chars

class Solution1 {
    struct _RecursionContext {
        size_t start_from{0};
        std::vector<size_t> indices_removed;
    };

    size_t iterations_count_{0};

  public:
    std::vector<std::string> removeInvalidParentheses(std::string s) {
        iterations_count_ = 0;

        // Two different deletes may result in the same result
        std::unordered_set<std::string> result_set;

        // Find minimum deletes required
        const auto min_del_req = _find_min_deletes_required(s);
        if (0 == min_del_req) {
            return {s};
        }

        // Prepare for recursive brute force
        std::deque<_RecursionContext> _stack;
        _stack.push_back(_RecursionContext{});
        for (; !_stack.empty();) {

            ++iterations_count_;

            // note. change stack to queue and you will have BFS instead of DFS
            auto ctx = std::move(_stack.front());
            _stack.pop_front();

            // a. check if we are happy with current deletes
            // Process only those deletes that are potentially good
            if (ctx.indices_removed.size() == min_del_req) {
                if (_is_well_formed(s, ctx.indices_removed)) {
                    result_set.insert(_remove_indices(s, ctx.indices_removed));
                }
            }

            // b. Check if we didn't exceed the count of deletes
            if (ctx.indices_removed.size() < min_del_req) {

                // We are going to test every available position where we have a
                // parenthesis char
                for (size_t pos = ctx.start_from; pos < s.size(); ++pos) {

                    if (s[pos] != '(' && s[pos] != ')') {
                        continue;
                    }

                    // Push to stack further possible deletes
                    _RecursionContext new_ctx;
                    new_ctx.indices_removed = ctx.indices_removed;
                    new_ctx.indices_removed.push_back(pos);
                    new_ctx.start_from = pos + 1;

                    _stack.push_back(std::move(new_ctx));
                }
            }
        }

        std::vector<std::string> result;
        for (auto &&str : result_set) {
            result.push_back(std::move(str));
        }
        return result;
    }

  private:
    bool _is_well_formed(const std::string &s) {
        int number_of_open_p{0};

        for (auto const ch : s) {
            if (ch == '(') {
                ++number_of_open_p;
            } else if (ch == ')') {
                --number_of_open_p;
                if (number_of_open_p < 0) {
                    return false;
                }
            }
        }

        return number_of_open_p == 0;
    }

    bool _is_well_formed(const std::string &s,
                         const std::vector<size_t> &skip_indices) {
        int number_of_open_p{0};
        auto it_pos = skip_indices.cbegin();

        size_t pos = 0;
        for (auto const ch : s) {

            if ((it_pos != skip_indices.cend() && *it_pos == pos++))
                [[unlikely]] {
                ++it_pos;
                continue;
            }
            if (ch == '(') {
                ++number_of_open_p;
            } else if (ch == ')') {
                --number_of_open_p;
                if (number_of_open_p < 0) {
                    return false;
                }
            }
        }

        return number_of_open_p == 0;
    }

    size_t _find_min_deletes_required(const std::string &s) noexcept {
        size_t violating_closures{0};
        size_t number_of_open_p{0};

        for (auto const ch : s) {
            if (ch == '(') {
                ++number_of_open_p;
            } else if (ch == ')') {
                if (number_of_open_p > 0) {
                    --number_of_open_p;
                } else {
                    violating_closures++;
                }
            }
        }

        // At the end we have:
        // `result` - number of closing parentheses that violate the correctness
        // of the expression `number_of_open_p` - number of non-closed
        // parentheses
        return violating_closures + number_of_open_p;
    }

    std::string _remove_indices(const std::string &s,
                                const std::vector<size_t> &indices) noexcept {
        // `indices` are considered to be sorted
        // TODO: there might be more optimal solution
        std::string result = s;
        for (auto it = indices.crbegin(); it != indices.crend(); ++it) {
            result.erase(*it, 1);
        }
        return result;
    }
};

class Solution2 {
    struct _RecursionContext {
        size_t start_from{0};
        std::string modified_string;
    };

    std::unordered_set<std::string> already_visited_;

  public:
    std::vector<std::string> removeInvalidParentheses(std::string s) {
        already_visited_.clear();

        // Two different deletes may result in the same result
        std::unordered_set<std::string> result_set;

        // Find minimum deletes required
        auto min_del_req = _find_min_deletes_required(s);
        if (0 == min_del_req) {
            return {s};
        }

        // Prepare for recursive brute force
        std::deque<_RecursionContext> _stack;
        _stack.push_back(_RecursionContext{0, s});
        for (; !_stack.empty();) {

            // note. change stack to queue and you will have BFS instead of DFS
            auto ctx = std::move(_stack.front());
            _stack.pop_front();

            // a. check if we are happy with current deletes
            // Process only those deletes that are potentially good
            auto deletes_count = s.size() - ctx.modified_string.size();
            if (deletes_count == min_del_req) {
                // Don't waste time processing strings with larger deletes
                // count
                if (_is_well_formed(ctx.modified_string)) {
                    result_set.insert(ctx.modified_string);
                }
            }

            // b. Check if we didn't exceed the number of deletes
            if (deletes_count < min_del_req) {

                // We are going to test every available position where we have a
                // parenthesis char
                for (size_t pos = ctx.start_from;
                     pos < ctx.modified_string.size(); ++pos) {

                    if (ctx.modified_string[pos] != '(' &&
                        ctx.modified_string[pos] != ')') {
                        continue;
                    }

                    // Push to stack further possible deletes
                    _RecursionContext new_ctx;
                    if (pos != 0) {
                        new_ctx.modified_string =
                            ctx.modified_string.substr(0, pos);
                    }
                    if ((pos + 1) < ctx.modified_string.size()) {
                        new_ctx.modified_string +=
                            ctx.modified_string.substr(pos + 1);
                    }
                    new_ctx.start_from = pos;

                    // Memoization in work. Skip those variants which were
                    // already processed
                    if (already_visited_.cend() ==
                        already_visited_.find(new_ctx.modified_string)) {
                        already_visited_.insert(new_ctx.modified_string);
                        _stack.push_back(std::move(new_ctx));
                    }
                }
            }
        }

        std::vector<std::string> result;
        for (auto &&str : result_set) {
            result.push_back(std::move(str));
        }
        return result;
    }

  private:
    static bool _is_well_formed(std::string_view s) {
        int number_of_open_p{0};

        for (auto const ch : s) {
            if (ch == '(') {
                ++number_of_open_p;
            } else if (ch == ')') {
                --number_of_open_p;
                if (number_of_open_p < 0) {
                    return false;
                }
            }
        }

        return number_of_open_p == 0;
    }

    static size_t _find_min_deletes_required(std::string_view s) noexcept {
        size_t violating_closures{0};
        size_t number_of_open_p{0};

        for (auto const ch : s) {
            if (ch == '(') {
                ++number_of_open_p;
            } else if (ch == ')') {
                if (number_of_open_p > 0) {
                    --number_of_open_p;
                } else {
                    violating_closures++;
                }
            }
        }

        // At the end we have:
        // `result` - number of closing parentheses that violate the correctness
        // of the expression;
        // `number_of_open_p` - number of non-closed parentheses.
        return violating_closures + number_of_open_p;
    }
};

namespace FibLoop {

[[nodiscard]] int fibNaive(int depth) {
    if (depth == 0 || depth == 1) {
        return depth;
    }

    int a = 0;
    int b = 1;
    int sum = 0;
    for (int step = 0; step < (depth - 1); ++step) {
        sum = a + b;
        a = b;
        b = sum;
    }
    return sum;
}

[[nodiscard]] int fibRec(int depth) {
    if (depth <= 1) {
        return depth;
    }
    return fibRec(depth - 1) + fibRec(depth - 2);
}

// A recursive fib(n) does:
//
// 1. If n <= 1 return n
// 2. Otherwise compute fib(n-1), then fib(n-2), then return sum.
//
// We simulate that with frames like:
//
// • in - argument
// • state - where we are in the function:
//     • e1: just entered
//     • e2: returned from fib(n-1) (so we have a)
//     • e3: returned from fib(n-2) (so we have b, can compute result)
//
// We also maintain a single variable retRegister that represents “the return
// value of the most recently completed frame”, like a CPU return register.

enum class FrameState : uint8_t {
    e1, // call fib(n-1)
    e2, // have result from e1, call fib(n-2)
    e3  // have result from e2, calculate and return a sum
};
struct FiboFrame {
    const int in; // input argument, depth

    FrameState state = FrameState::e1;

    int lFib = 0; // result of fib(in - 1)
    int rFib = 0; // result of fib(in - 2)

    // NOTE: We don't store a result here, `retRegister` passes result to the
    // callee frame, which is next on the stack.
};

[[nodiscard]] int fibFrames(int depth) {

    std::stack<FiboFrame> frames;
    int retRegister = 0; // fib(b) return value

    // Make a very first call. The return from this call is an overall result of
    // the `fibFrames` function.
    frames.emplace(depth);
    while (!frames.empty()) {
        auto &f = frames.top();

        switch (f.state) {
        case FrameState::e1: // call fib(n-1)
            if (f.in <= 1) {
                retRegister = f.in;
                frames.pop(); // immediate "Return" from function
            } else {
                f.state = FrameState::e2;
                frames.emplace(f.in - 1);
            }
            break;
        case FrameState::e2: // call fib(n-2)
            f.lFib = retRegister;
            if (f.in <= 1) {
                retRegister = f.in;
                frames.pop(); // immediate "Return" from function
            } else {
                f.state = FrameState::e3;
                frames.emplace(f.in - 2);
            }
            break;
        case FrameState::e3: // calculate sum and return
            f.rFib = retRegister;
            retRegister = f.lFib + f.rFib;
            frames.pop(); // "Return" from function
            break;
        }
    }
    return retRegister;
}

} // namespace FibLoop

int main(void) {
    [[maybe_unused]] Solution1 sol1;
    Solution2 sol2;

    // clang-format off
    std::vector< std::string > test_asset{
        "(a)())()",
        ")(",
        ")((())))))()(((l((((",
        ")()()(a",
        ")((())))))()(((l((((",
        "(((((((((((((((((((((((((((((((((((aaaaa"
    };
    // clang-format on

    for (const auto &str : test_asset) {

        // auto result1 = sol1.removeInvalidParentheses(str);
        auto result2 = sol2.removeInvalidParentheses(str);
        // assert(result1 == result2);
    }

    assert(FibLoop::fibNaive(15) == FibLoop::fibRec(15));
    assert(FibLoop::fibNaive(15) == FibLoop::fibFrames(15));

    return 0;
}
