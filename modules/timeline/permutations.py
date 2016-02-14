import itertools

input_str = 'abc'

perms = itertools.permutations(input_str, len(input_str))

# Returns an array of tuples of all the combinations you can have using 1 or 2 arms
# for a certain order permutation
# perm is a tuple of the current permutation like: ('a', 'b', 'c')
def sliding_window(perm):
    ret = [perm]
    temp_str = perm
    i = 0
    while (i < len(perm)):
        left_substr = temp_str[0:i]
        # Like ab, out of abcde
        # Or bc out of abcde
        cur_substr = (''.join(temp_str[i:i+2]), ) # Single elem tuple like 'ab'
        right_substr_combs = sliding_window(temp_str[i+2:])

        for comb in right_substr_combs:
            new_comb = left_substr + cur_substr + comb
            if new_comb not in ret:
                ret.append(left_substr + cur_substr + comb)
        i += 1
    return ret