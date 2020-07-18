#!/usr/bin/python3

class Rule:
	def __init__ (o, name, prod, action = None):
		o.name = name
		o.prod = prod
		o.action = action

	def rule_str (o):
		rhs = ' '.join (o.prod) if o.prod else 'ε'

		return o.name + ' → ' + rhs

	def __repr__ (o):
		rule = o.rule_str ()

		if not isinstance (o.action, list):
			return rule

		action = ' '.join (map (str, o.action))
		return '{:20} : {}'.format (rule, action)

class Grammar:
	def __init__ (o, rules, verbose = False):
		o.rules = rules

		for i, r in enumerate (rules):
			if r.action is None:
				r.action = 'f' + str (i)

		o.start = o.rules[0].name
		o.names = set (r.name for r in rules)
		o.terms = set (s for r in rules for s in r.prod) - o.names

		if verbose:
			o.show ()

	def show (o, indent = 4):
		print ('rules:\n')

		for i, r in enumerate (o.rules):
			print (' ' * indent + str (i) + ': ' + str (r))

		print ()
		print ('terminals:', ' '.join (sorted (o.terms)))
		print ()

class GSet:
	def __init__ (o):
		o.FS = {}

	def __getitem__(o, s):
		return o.FS [s]

	def __repr__ (o):
		return repr (o.FS)

	def show (o, s = None, indent = 4):
		if s is None:
			for s in sorted (o.FS.keys ()):
				o.show (s, indent)

			return

		prefix = ' ' * indent + s + ' = '

		def fn (s):
			return 'ε' if s is None else '$' if s == 0 else s

		print (prefix + ' '.join (sorted (map (fn, o.FS[s]))))

class First (GSet):
	def __init__ (o, g, verbose = False):
		if not isinstance (g, Grammar):
			raise ValueError ('Grammar required')

		super().__init__ ()

		for s in g.terms:
			o.FS [s] = set ([s])

		for s in g.names:
			o.FS [s] = set ()

		def step ():
			count = 0

			for r in g.rules:
				F = o.calc (r.prod)
				S = o.FS [r.name]

				count += len (F - S)
				S |= F

			return count

		while step () > 0:
			pass

		for s in g.names:
			if not o.FS [s]:
				raise ValueError ('{} is incomplete'.format (s))

		if verbose:
			print ('first:\n')
			o.show ()
			print ()

	def calc (o, seq):
		S = set ()

		for s in seq:
			F = o [s]

			if not None in F:
				return S | F

			S |= F
			S.remove (None)

		S.add (None)
		return S

class Follow (GSet):
	def __init__ (o, g, first, verbose = False):
		if not isinstance (g, Grammar):
			raise ValueError ('Grammar required')

		if not isinstance (first, First):
			raise ValueError ('First set required')

		super().__init__ ()

		for n in g.names:
			o.FS [n] = set ()

		o.FS [g.start].add (0)

		def step ():
			count = 0

			for r in g.rules:
				prod = r.prod

				while prod:
					head, *prod = prod

					if not head in g.names:
						continue

					F = first.calc (prod)

					if None in F:
						F.remove (None)
						F |= o.FS [r.name]

					S = o.FS [head]
					count += len (F - S)
					S |= F

			return count

		while step () > 0:
			pass

		if verbose:
			print ('follow:\n')
			o.show ()
			print ()

class LL1_Table:
	def __init__ (o, rules, verbose = False):
		o.grammar = g = Grammar (rules, verbose)

		first  = First  (o.grammar, verbose)
		follow = Follow (o.grammar, first, verbose)

		o.T = T = {}

		for n in g.names:
			T[n] = {}

		def add (i, n, s):
			if s in T[n]:
				reason = 'conflict {} with {}'.format (n, s)
				raise ValueError (reason)

			T[n][s] = i

		for i, r in enumerate (g.rules):
			fs = first.calc (r.prod)

			for s in fs:
				add (i, r.name, s)

			if None in fs:
				for s in follow [r.name]:
					add (i, r.name, s)

		if verbose:
			o.show ()

	def show (o):
		g = o.grammar
		T = o.T

		print ('table:\n')
		print ('    ', end = '')

		for i in sorted (g.terms):
			print ('{:>4}'.format (i), '', end = '')

		print ('   $')
		print ('   +', end = '')

		for i in sorted (g.terms):
			print ('----+', end = '')

		print ('----+')

		for n in sorted (g.names):
			print ('{:2} |'.format (n), end = '')

			for i in sorted (g.terms) + [0]:
				x = T[n][i] if i in T[n] else '·'

				print ('{:>4}'.format (x), '', end = '')

			print ()

		print ()

class LL1 (LL1_Table):
	def __init__ (o, rules, verbose = False):
		super().__init__ (rules, verbose)

	def pop (o):
		if o.verbose:
			def fn (s):
				return '$' if s == 0 else str (s)

			st = ' '.join (map (fn, reversed (o.stack)))
			print ('stack: {:40}'.format (st), end = '')

		return o.stack.pop ()

	def apply (o, s):
		try:
			i = o.T[s][o.token]
			r = o.grammar.rules[i]
		except KeyError:
			raise ValueError ('Syntax Error')

		if o.verbose:
			print (': apply ', r.rule_str ())

		o.stack.extend (reversed (r.prod))
		return i

	def accept (o):
		if o.verbose:
			print (': accept', o.token)

		ret = o.token
		o.token = next (o.prog)
		return ret

	def start (o, prog, verbose = False):
		o.prog    = prog
		o.verbose = verbose
		o.stack   = [0, o.grammar.start]
		o.token   = next (prog)

	def __next__ (o):
		s = o.pop ()

		if s in o.grammar.names:
			return o.apply (s)

		if s == o.token:
			return o.accept ()

		reason = 'Expect {}, got {}'.format (s, token)
		raise ValueError (reason)

	def make_ast (o):
		s = next (o)

		if not isinstance (s, int):
			return s

		r = o.grammar.rules[s]
		args = []

		for i in r.prod:
			args.append (o.make_ast ())

		if isinstance (r.action, list):
			def fn (x):
				if isinstance (x, int):
					return args[x]

				return x

			v = list (map (fn, r.action))

			return v[0] if len (v) == 1 else v

		return [r.action] + args

	def parse (o, prog, verbose = False):
		def fn (prog):
			for token in prog:
				if not token in o.grammar.terms:
					reason = 'Unknown token ' + str (token)
					raise ValueError (reason)

				yield token

			yield 0

		o.start (fn (prog), verbose)
		ast = o.make_ast ()

		if o.token != 0:
			raise ValueError ('Extra tokens at end')

		return ast

def se_str (o):
	if not isinstance (o, list):
		return str (o)

	return '(' + ' '.join (map (se_str, o)) + ')'

rules_lr = [
	Rule ("E", ["T"],		[0]),
	Rule ("E", ["T", "+", "E"],	["+", 0, 2]),

	Rule ("T", ["F"],		[0]),
	Rule ("T", ["F", "*", "T"],	["*", 0, 2]),

	Rule ("F", ["n"],		[0]),
	Rule ("F", ["(", "E", ")"],	[1])
]

rules_ll = [
	Rule ("E", ["T", "R"],		["+", 0, 1]),
	Rule ("R", [],			["ε"]),
	Rule ("R", ["+", "E"],		[1]),

	Rule ("T", ["F", "S"],		["*", 0, 1]),
	Rule ("S", [],			["ε"]),
	Rule ("S", ["*", "T"],		[1]),

	Rule ("F", ["n"],		[0]),
	Rule ("F", ["(", "E", ")"],	[1])
]

m = LL1 (rules_ll, True)

prog = ['n', '+', 'n', '*', '(', 'n', ')']

print ('parse ' + ' '.join (prog) + ':\n')

try:
	ast = m.parse (prog, True)
	print (se_str (ast))

except ValueError as e:
	print ()
	print ('E:', e)
