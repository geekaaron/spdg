# spdg
A simple password dictionary generator

# Usage

```
Usage: spdg <min> <max> <option[s]>
	-d, --digit			- 0 - 9
	-z, --lower			- a - z
	-Z, --upper			- A - Z
	-s, --symbol			- +-*/\=@#$%^&?'"`~()[]{}:;,.
	-S, --specify charset		- Use specified charset
	-i, --infos str1, str2...	- Generate password based on informations
	-h, --help			- Show help message
```

# Test

Generate password consisting of numbers.

```
$ spdg 3 4 -d
```

Generate password consisting of numbers & letters.

```
$ spdg 3 4 -d -z -Z
```

Generate password consisting of all characters.

```
$ spdg 3 4 -d -z -Z -s
```

Generate password consisting of specified charset.

```
$ spdg 3 4 -S abc123
```

Generate password consisting of specified information.

```
$ spdg 3 4 -i Tom 2001 Atlantis apple
```

# End

Any problems: for_unity@sina.com