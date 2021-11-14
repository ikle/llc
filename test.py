#!/usr/bin/python3

EOI = '$'

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

	def map_args (o, args):
		if isinstance (o.action, list):
			def fn (x):
				if isinstance (x, int):
					return args[x]

				return x

			v = list (map (fn, o.action))

			return v[0] if len (v) == 1 else v

		return [o.action] + args

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
			return 'ε' if s is None else s

		print (prefix + ' '.join (sorted (map (fn, o.FS[s]))))

class First (GSet):
	def __init__ (o, g, verbose = False):
		if not isinstance (g, Grammar):
			raise ValueError ('Grammar required')

		super().__init__ ()

		o.FS [EOI] = set ([EOI])

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

		o.FS [g.start].add (EOI)

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

			for i in sorted (g.terms) + [EOI]:
				x = T[n][i] if i in T[n] else '·'

				print ('{:>4}'.format (x), '', end = '')

			print ()

		print ()

class LL1 (LL1_Table):
	def __init__ (o, rules, verbose = False):
		super().__init__ (rules, verbose)

	def apply (o, s):
		if o.verbose:
			print (': {} at {} '.format (s, o.la), end = '')

		try:
			i = o.T[s][o.la]
			r = o.grammar.rules[i]
		except KeyError:
			reason = 'parse {}, got {}'.format (s, o.la)
			raise ValueError ('Syntax Error: ' + reason)

		if o.verbose:
			print (': apply ' + r.rule_str ())

		o.stack.extend (reversed (r.prod))

		return r.map_args ([o.make_ast () for _ in r.prod])

	def accept (o):
		if o.verbose:
			print (': accept', o.la)

		token, o.la = o.la, next (o.prog)
		return token

	def make_ast (o):
		if o.verbose:
			st = ' '.join (reversed (o.stack))
			print ('stack: {:40}'.format (st), end = '')

		s = o.stack.pop ()

		return o.accept () if s == o.la else o.apply (s)

	def parse (o, prog, verbose = False):
		def fn (prog):
			for token in prog:
				if not token in o.grammar.terms:
					reason = 'Unknown token ' + str (token)
					raise ValueError (reason)

				yield token

			yield EOI

		o.prog    = fn (prog)
		o.verbose = verbose
		o.stack   = [EOI, o.grammar.start]
		o.la      = next (o.prog)

		ast = o.make_ast ()

		if o.la != EOI:
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

print ()

class LR:
	def __init__ (o, rules, verbose = False):
		arules = [Rule ("γ", [rules[0].name], [0])] + rules
		o.grammar = Grammar (arules, verbose)

		o.first  = First  (o.grammar, verbose)
		o.follow = Follow (o.grammar, o.first, verbose)

		o.map   = {}	# item set to state index map
		o.count = 0	# total number of states in map
		o.trans = {}	# shift FSM
		o.reducts = {}	# reduce action map

	def add_state (o, S):
		I = o.item_set_close (S)

		if I in o.map:
			return o.map[I]

		i = o.map[I] = o.count
		o.count += 1
		o.reducts[i] = o.count_reducts (I)
		o.trans[i] = o.make_trans (I)
		o.process_conficts (i)
		return i

	def process_conficts (o, i):
		T = o.trans[i]
		R = o.reducts[i]

		if T.keys () & R.keys ():
			reason = 'SR conflict in {}'.format (i)
			raise ValueError (reason)

	def item_str (o, it):
		i, pos = it
		r = o.grammar.rules[i]
		l = r.prod[:pos] + ['•'] + r.prod[pos:]

		rhs = ' '.join (l) if r.prod else '• ε'

		return r.name + ' → ' + rhs

	def item_set_show (o, S, indent = 4):
		for e in sorted (S):
			print (' ' * indent + o.item_str (e))

		print ()

	def show (o):
		M = {}

		for s in o.map:
			i = o.map[s]
			M[i] = s

		for i in sorted (M):
			print ('state ' + str (i) + ':\n')
			o.item_set_show (M[i])

			T = o.trans[i]

			if T:
				print ('    transitions:\n')

				for s in sorted (T):
					print ('       ', s, '→', T[s])

				print ()

			T = o.reducts[i]

			if T:
				print ('    reducts:\n')

				for s in sorted (T):
					print ('       ', s, '→', T[s])

				print ()

	# LR Parser Interpretator

	def start (o, prog, verbose = False):
		o.verbose = verbose

		def fn (prog):
			for token in prog:
				if not token in o.grammar.terms:
					reason = 'Unknown token ' + str (token)
					raise ValueError (reason)

				yield token

			yield EOI

		o.prog = fn (prog)

		o.symbols = [EOI]	# symbol stack for verbose mode only
		o.args    = [EOI]	# AST node stack
		o.states  = [EOI, 0]	# state stack
		o.la      = next (o.prog)

	def shift (o):
		if o.verbose:
			print ('shift  {},\t'.format (o.la), end = '')

		token = o.la
		o.args.append (token)
		o.la = next (o.prog)
		return token

	def reduce (o, la):
		i = o.states[-1]
		R = o.reducts[i]

		if not la in R.keys ():
			return None

		r = o.grammar.rules[R[la]]
		ast = r.map_args (o.args[-len (r.prod):])

		for i in range (len (r.prod)):
			o.symbols.pop ()  # verbose mode
			o.args.pop ()
			o.states.pop ()

		o.args.append (ast)

		if o.verbose:
			print ('reduce {},\t'.format (r.rule_str ()), end = '')

		return r.name

	def goto (o, top):
		i = o.states[-1]
		T = o.trans[i]

		if not top in T:
			raise ValueError ('Syntax error')

		if o.verbose:
			print ('goto', T[top])

		o.symbols.append (top)  # verbose mode
		o.states.append (T[top])

	def parse (o, prog, verbose = False):
		o.start (prog, verbose)

		while True:
			if o.verbose:
				st = ' '.join (o.symbols)
				print ('stack: {:40}'.format (st), end = '')

			top = o.reduce (o.la)

			if top == None:
				top = o.shift ()

			if top == o.grammar.start:
				if o.verbose:
					print ('accept')

				break

			o.goto (top)

		if o.la != EOI:
			raise ValueError ('Extra tokens at end')

		return o.args[-1]

class SLR (LR):
	def __init__ (o, rules, verbose = False):
		super().__init__ (rules, verbose)

		o.add_state ({(0, 0)})

		if verbose:
			o.show ()

	def item_set_close (o, K):
		S = K.copy ()
		Q = list (S)

		for i, pos in Q:
			r = o.grammar.rules[i]

			if pos >= len (r.prod):
				continue

			s = r.prod[pos]

			if not s in o.grammar.names:
				continue

			for idx, r in enumerate (o.grammar.rules):
				if s == r.name:
					e = (idx, 0)

					if not e in S:
						S.add (e)
						Q.append (e)

		return frozenset (S)

	def count_reducts (o, S):
		R = {}

		for i, pos in S:
			r = o.grammar.rules[i]

			if pos == len (r.prod):
				for s in o.follow[r.name]:
					if s in R and R[s] != i:
						fmt = 'RR conflict {} with {}'
						reason = fmt.format (i, R[s])
						raise ValueError (reason)

					R[s] = i

		return R

	def make_trans (o, S):
		T = {}

		for i, pos in S:
			r = o.grammar.rules[i]

			if pos < len (r.prod):
				s = r.prod[pos]

				if not s in T:
					T[s] = set ()

				T[s].add ((i, pos + 1))

		A = {}

		# Note: sorted to make reproducible FSM

		for s in sorted (T.keys ()):
			A[s] = o.add_state (T[s])

		return A

rules_slr = [
	Rule ("E", ["T"],		[0]),
	Rule ("E", ["E", "+", "T"],	["+", 0, 2]),
	Rule ("T", ["F"],		[0]),
	Rule ("T", ["T", "*", "F"],	["*", 0, 2]),
	Rule ("F", ["n"],		[0]),
	Rule ("F", ["(", "E", ")"],	[1])
]

m = SLR (rules_slr, True)

print ('transitions:', m.trans)
print ('reducts:', m.reducts)

prog = ['n', '+', 'n', '*', '(', 'n', ')']

print ('parse ' + ' '.join (prog) + ':\n')

try:
	ast = m.parse (prog, True)
	print (se_str (ast))

except ValueError as e:
	print ()
	print ('E:', e)

class LR1 (LR):
	def __init__ (o, rules, verbose = False):
		super().__init__ (rules, verbose)

		o.add_state ({(0, 0, EOI)})

		if verbose:
			o.show ()

	def item_str (o, it):
		i, pos, la = it
		r = o.grammar.rules[i]
		l = r.prod[:pos] + ['•'] + r.prod[pos:]

		rhs = ' '.join (l) if r.prod else '• ε'

		return '{:20} : {}'.format (r.name + ' → ' + rhs, la)

	def item_set_close (o, K):
		S = K.copy ()
		Q = list (S)

		for i, pos, la in Q:
			r = o.grammar.rules[i]

			if pos >= len (r.prod):
				continue

			s = r.prod[pos]

			if not s in o.grammar.names:
				continue

			tail = r.prod[pos + 1:] + [la]
			FS = o.first.calc (tail)

			for idx, r in enumerate (o.grammar.rules):
				if s == r.name:
					for la in FS:
						e = (idx, 0, la)

						if not e in S:
							S.add (e)
							Q.append (e)

		return frozenset (S)

	def count_reducts (o, S):
		R = {}

		for i, pos, la in S:
			r = o.grammar.rules[i]

			if pos == len (r.prod):
				if la in R and R[la] != i:
					fmt = 'RR conflict {} with {}'
					reason = fmt.format (i, R[la])
					raise ValueError (reason)

				R[la] = i

		return R

	def make_trans (o, S):
		T = {}

		for i, pos, la in S:
			r = o.grammar.rules[i]

			if pos < len (r.prod):
				s = r.prod[pos]

				if not s in T:
					T[s] = set ()

				T[s].add ((i, pos + 1, la))

		A = {}

		# Note: sorted to make reproducible FSM

		for s in sorted (T.keys ()):
			A[s] = o.add_state (T[s])

		return A

m = LR1 (rules_slr, True)

print ('transitions:', m.trans)
print ('reducts:', m.reducts)

prog = ['n', '+', 'n', '*', '(', 'n', ')']

print ('parse ' + ' '.join (prog) + ':\n')

try:
	ast = m.parse (prog, True)
	print (se_str (ast))

except ValueError as e:
	print ()
	print ('E:', e)
