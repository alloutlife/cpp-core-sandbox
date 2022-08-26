// This playground demonstrates how to organise a recursion without recursive
// function call

// Consider solving a bit tricky leetcode problem
// [url](https://leetcode.com/problems/remove-invalid-parentheses/)

// The approach to finding a solution for this task is:
// 1. find minimum modifications required to make the expression valid
// 2. brute force the string by removing each parenthesis and checking if
// modified string is good

#include <algorithm>
#include <cassert>
#include <deque>
#include <string>
#include <unordered_set>
#include <vector>

// Ideas for optimization:
// - We can count a minimum number of changes required to make string valid,
// so we don't need to proces those strings that contain differrent number of
// changes.
// - different removals may result in the same string to process. Use
// memoization in order to skip already processed strings.
// - there is no need to test removals of non-parentheses chars

class Solution1
{
    struct _RecursionContext {
        size_t start_from{0};
        std::vector<size_t> indices_removed;
    };

    size_t iterations_count_{0};

  public:
    std::vector<std::string> removeInvalidParentheses(std::string s)
    {
        iterations_count_ = 0;

        // Two different modifications may result in the same result
        std::unordered_set<std::string> result_set;

        // Find minimum modifications required
        auto min_mods_req = _find_min_modifications_required(s);
        if (0 == min_mods_req) {
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

            // a. check if we are happy with current modifications
            // Process only those modifications that are potentially good
            if (ctx.indices_removed.size() == min_mods_req) {
                if (_is_well_formed(s, ctx.indices_removed)) {
                    result_set.insert(_remove_indices(s, ctx.indices_removed));
                }
            }

            // b. Check if we didn't exceed the count of modifications
            if (ctx.indices_removed.size() < min_mods_req) {

                // We are going to test every available position where we have a
                // parenthesis char
                for (size_t pos = ctx.start_from; pos < s.size(); ++pos) {

                    if (s[pos] != '(' && s[pos] != ')') {
                        continue;
                    }

                    // Push to stack further possible modifications
                    _RecursionContext new_ctx;
                    new_ctx.indices_removed = ctx.indices_removed;
                    new_ctx.indices_removed.push_back(pos);
                    new_ctx.start_from = pos + 1;

                    _stack.push_back(std::move(new_ctx));
                }
            }
        }

        std::vector<std::string> result;
        for (const auto &str : result_set) {
            result.push_back(std::move(str));
        }
        return result;
    }

  private:
    bool _is_well_formed(const std::string &s)
    {
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
                         const std::vector<size_t> &skip_indices)
    {
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

    size_t _find_min_modifications_required(const std::string &s) noexcept
    {
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
                                const std::vector<size_t> &indices) noexcept
    {
        // `indices` are considered to be sorted
        // TODO: there might be more optimal solution
        std::string result = s;
        for (auto it = indices.crbegin(); it != indices.crend(); ++it) {
            result.erase(*it, 1);
        }
        return result;
    }
};

class Solution2
{
    struct _RecursionContext {
        size_t start_from{0};
        std::string modified_string;
    };

    std::unordered_set<std::string> already_visited_;

  public:
    std::vector<std::string> removeInvalidParentheses(std::string s)
    {
        already_visited_.clear();

        // Two different modifications may result in the same result
        std::unordered_set<std::string> result_set;

        // Find minimum modifications required
        auto min_mods_req = _find_min_modifications_required(s);
        if (0 == min_mods_req) {
            return {s};
        }

        // Prepare for recursive brute force
        std::deque<_RecursionContext> _stack;
        _stack.push_back(_RecursionContext{0, s});
        for (; !_stack.empty();) {

            // note. change stack to queue and you will have BFS instead of DFS
            auto ctx = std::move(_stack.front());
            _stack.pop_front();

            // a. check if we are happy with current modifications
            // Process only those modifications that are potentially good
            auto modifications_count = s.size() - ctx.modified_string.size();
            if (modifications_count == min_mods_req) {
                // Don't waste time processing strings with different
                // modifications count
                if (_is_well_formed(ctx.modified_string)) {
                    result_set.insert(ctx.modified_string);
                }
            }

            // b. Check if we didn't exceed the number of modifications
            if (modifications_count < min_mods_req) {

                // We are going to test every available position where we have a
                // parenthesis char
                for (size_t pos = ctx.start_from;
                     pos < ctx.modified_string.size(); ++pos) {

                    if (ctx.modified_string[pos] != '(' &&
                        ctx.modified_string[pos] != ')') {
                        continue;
                    }

                    // Push to stack further possible modifications
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
        for (const auto &str : result_set) {
            result.push_back(std::move(str));
        }
        return result;
    }

  private:
    bool _is_well_formed(const std::string &s)
    {
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

    size_t _find_min_modifications_required(const std::string &s) noexcept
    {
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

int main(void)
{
    Solution1 sol1;
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

    return 0;
}
