// Example0.cpp : Defines the entry point for the console application.
//

#include <cmath>
#include <iostream>
#include <string>
#include <Eigen/Dense>
#include <memory>
#include <iomanip>
#include <vector>
#include <unordered_map>

using Vector_ty = Eigen::VectorXd;
using Matrix_ty = Eigen::MatrixXd;

namespace Implementation0{



/*
		f(a,b,c) = ( w - w0 )^2
		w(a,b,c) = sin(a b) + c b^2 + a^3 c^2
*/

struct Computer {
	Matrix_ty MakeDiffU(double a, double b, double c) {

	

		auto result = Matrix_ty(4, 3);
		result.fill(0);
		result(0, 0) = 1;
		result(1, 1) = 1;
		result(2, 2) = 1;
		result(3, 0) = b * std::cos(a * b) + 3 * a * a * c * c;
		result(3, 1) = a * std::cos(a *b) + 2 * c * b;
		result(3, 2) = b * b + 2 * a * a * a * c;
		return result;
	}
	Matrix_ty MakeDiffV(double a, double u) {

		auto result = Matrix_ty(1, 2);
		result(0, 0) = 2 * a;
		result(0, 1) = 2 * u * std::exp(u*u - 1);
		return result;
	}
	Matrix_ty MakeDiffW(double c, double v) {

		auto result = Matrix_ty(1, 2);
		result(0, 0) = -2 * c * std::sin(c*c - 1);
		result(0, 1) = 2 * v / (v * v + 1);
		return result;
	}
	Matrix_ty MakeDiffF(double w, double w0) {

		auto result = Matrix_ty(1, 1);
		result(0, 0) = 2 * (w - w0);

		return result;
	}
};

struct Functional {
	double Eval(double a, double b, double c, double w0)const{
		auto u = std::sin(a*b) + c * b * b + a*a*a*c*c;
		auto v = std::exp(u*u - 1) + a*a;
		auto w = std::log(v*v + 1) + std::cos(c*c - 1);
		auto f = std::pow(w - w0, 2.0);
		return f;
	}
};

struct FiniteDiff {
	explicit FiniteDiff(double epsilon) :epsilon_{ epsilon } {}
	double d_a(double a, double b, double c, double w0)const {
		double lower = f_.Eval(a - epsilon_ / 2, b, c, w0);
		double upper = f_.Eval(a + epsilon_ / 2, b, c, w0);
		return (upper - lower) / epsilon_;
	}
private:
	Functional f_;
	double epsilon_;
};

struct ForwardDiff {
	double d_a(double a, double b, double c, double w0)const {

		auto u = std::sin(a*b) + c * b * b + a*a*a*c*c;
		auto v = std::exp(u*u - 1) + a*a;
		auto w = std::log(v*v + 1) + std::cos(c*c - 1);


		auto C = Computer{};

		auto u_M = C.MakeDiffU(a, b, c);
		auto u_V = Vector_ty(3);
		u_V(0) = 1.0;
		u_V(1) = 0.0;
		u_V(2) = 0.0;

		//std::cerr << "u_M=\n" << u_M << "\n";
		
		auto d_u = u_M * u_V;



		auto v_M = C.MakeDiffV(a, u);
		auto v_V = Vector_ty(2);
		v_V(0) = d_u(0);
		v_V(1) = d_u(3);

		//std::cerr << "v_M=\n" << v_M << "\n";

		auto d_v = v_M * v_V;




		auto w_M = C.MakeDiffW(c, v);
		auto w_V = Vector_ty(2);
		w_V(0) = d_u(2);
		w_V(1) = d_v(0);

		//std::cerr << "w_M=\n" << w_M << "\n";

		auto d_w = w_M * w_V;

		auto f_M = C.MakeDiffF(w, w0);

		//std::cerr << "f_M=\n" << f_M << "\n";

		auto f_V = Vector_ty(1);
		f_V(0) = d_w(0);
		auto f_u = f_M * f_V;

		return f_u(0);
	}
};

struct BetterForwardDiff {
	double d_a(double a, double b, double c, double w0)const {

		auto u = std::sin(a*b) + c * b * b + a*a*a*c*c;
		auto v = std::exp(u*u - 1) + a*a;
		auto w = std::log(v*v + 1) + std::cos(c*c - 1);

		auto d_aa = 1.0;
		auto d_b = 0.0;
		auto d_c = 0.0;

		auto C = Computer{};

		auto u_M = C.MakeDiffU(a, b, c);
		std::cout << u_M << "\n";
		auto u_V = Vector_ty(3);
		u_V(0) = d_aa;
		u_V(1) = 0.0;
		u_V(2) = 0.0;

		auto eval_u = u_M * u_V;
		std::cout << "eval_u\n" << eval_u << "\n";

		auto d_u = eval_u(3);

		auto v_M = C.MakeDiffV(a, u);
		std::cout << v_M << "\n";
		auto v_V = Vector_ty(2);
		v_V(0) = d_aa;
		v_V(1) = d_u;

		auto eval_v = v_M * v_V;
		std::cout << "eval_v\n" << eval_v << "\n";
		auto d_v = eval_v(0);

		auto w_M = C.MakeDiffW(c, v);
		auto w_V = Vector_ty(2);
		w_V(0) = d_c;
		w_V(1) = d_v;
		
		auto eval_d_w = w_M * w_V;
		std::cout << "eval_d_w\n" << eval_d_w << "\n";


		auto f_M = C.MakeDiffF(w, w0);

		auto f_V = Vector_ty(1);
		f_V(0) = eval_d_w(0);
		auto f_u = f_M * f_V;

		std::cout << "f_V\n" << f_V << "\n";
		std::cout << "f_u\n" << f_u << "\n";

		auto d = Vector_ty(7);
		d(0) = d_aa;
		d(1) = d_b;
		d(2) = d_c;
		d(3) = d_u;
		d(4) = d_v;
		d(5) = eval_d_w(0);
		d(6) = f_u(0);

		std::cout << "d=\n" << d << "\n";

		return f_u(0);
	}
};



void TestDriver()
{
	auto f = Functional{};
	auto fd = FiniteDiff(0.00001);
	auto ad = ForwardDiff{};
	auto better_ad = BetterForwardDiff{};

	auto a = 0.1;
	auto b = 0.1;
	auto c = 3.0;
	auto constexpr w0 = 2.0;

	std::cout << "f -> " << f.Eval(a, b, c, w0) << "\n";
	std::cout << "D_{fd}(f,a) -> " << fd.d_a(a, b, c, w0) << "\n";
	std::cout << "D_{ad}(f,a) -> " << ad.d_a(a, b, c, w0) << "\n";
	std::cout << "D_{ad}(f,a) -> " << better_ad.d_a(a, b, c, w0) << "\n";

}

} // end namespace Implementation0

  // 
namespace CandyTestFramework {
	struct SourceInfo {
		std::string File;
		std::string Function;
		int Line;
		friend std::ostream& operator<<(std::ostream& ostr, SourceInfo const& self) {
			ostr << "{file=" << self.File << ":" << self.Line << ", "
				<< "Function=" << self.Function << "}";
			return ostr;
		}
	};
#define SOURCE_INFO() ::CandyTestFramework::SourceInfo{ __FILE__, __FUNCTION__, __LINE__}
#define WITH_EXPR(expr) expr, #expr
#define CheckEqWithExpr(a,b) CheckEq(SOURCE_INFO(), WITH_EXPR(a), WITH_EXPR(b))
	struct CandyTestContext;
	struct CandyTestContextUnit {
		explicit CandyTestContextUnit(CandyTestContext* ctx) :ctx_{ ctx } {}
		CandyTestContext* Context()const { return ctx_;  }
		virtual ~CandyTestContextUnit() = default;
		virtual void CheckEq(SourceInfo const& src, double value, std::string const& value_literal,
			double expected, std::string const& expected_literal) = 0;
		
	private:
		CandyTestContext* ctx_;
	};
	struct CandyTestContext {
		virtual ~CandyTestContext() = default;
		virtual std::shared_ptr<CandyTestContextUnit> BuildUnit(std::string const& name,
			SourceInfo const& info) = 0;
		virtual void Emit(std::string const& msg) = 0;
		virtual void EmitFailure(std::string const& msg) {
			Emit(msg);
		}
	};

	struct CheckFailed : std::exception {
		CheckFailed(std::string const& msg) :msg_{ msg } {}
		std::string msg_;
		virtual const char* what() const noexcept {
			return msg_.c_str();
		}
	};

	static CandyTestContext* DefaultContext() {
		struct UnitImpl : CandyTestContextUnit {
			UnitImpl(CandyTestContext* ctx, std::string const& name,
				SourceInfo const& source) : CandyTestContextUnit{ ctx }, name_ {
				name
			}, source_{} {}
			virtual void CheckEq(SourceInfo const& src, double value, std::string const& value_literal,
				double expected, std::string const& expected_literal) {
				double const epsilon = 0.00001;
				auto cond = (std::fabs(value - expected) < epsilon);
				if (!cond) {
					std::stringstream ss;
					ss << "===========[" << name_ << "]============\n";
					ss << "assertion failed at " << src << "\n";
					ss << "    " << value_literal << "=" << value << "\n";
					ss << "    " << expected_literal << "=" << expected;
					Context()->EmitFailure(ss.str());
				}
				else {
					std::stringstream ss;
					ss << "===========[" << name_ << "]============\n";
					ss << "assertion sussess at " << src << "\n";
					ss << "    " << value_literal << "=" << value << "\n";
					ss << "    " << expected_literal << "=" << expected;
					Context()->Emit(ss.str());
				}
			}
		private:
			std::string name_;
			SourceInfo source_;
		};
		struct Impl : CandyTestContext {
			virtual std::shared_ptr<CandyTestContextUnit> BuildUnit(std::string const& name,
				SourceInfo const& info)override {
				return std::make_shared<UnitImpl>(this, name, info);
			}
			virtual void Emit(std::string const& msg)override {
				//std::cerr << msg << "\n";
			}
			virtual void EmitFailure(std::string const& msg)override{
				std::cerr << msg << "\n";
			}
		};
		static Impl memory;
		return &memory;
	}



	struct Unit {
		virtual ~Unit() = default;
		explicit Unit(std::string const& name, SourceInfo const& src) :name_{ name }, src_{ src } {
			GetRegister()->push_back(this);
		}
		std::string const& Name()const {
			return name_;
		}
		SourceInfo const& Source()const { return src_; }
		virtual void RunUnit(CandyTestContextUnit* unit)const = 0;

		static std::vector<Unit*>* GetRegister() {
			static std::vector<Unit*> mem;
			return &mem;
		}
		static void RunAll(CandyTestContext* ctx) {
			for (auto const& unit_ptr : *GetRegister()) {
				std::cerr << "Running " << unit_ptr->Name() << "\n";

				auto unit = ctx->BuildUnit(unit_ptr->Name(), unit_ptr->Source());
				unit_ptr->RunUnit(unit.get()); 
			}
		}
	private:
		std::string name_;
		SourceInfo src_;
	};
	
        template<class T>
	struct RegisterUnitTest {
		RegisterUnitTest() {
			Unit::GetRegister()->push_back(new T());
		}
	};


} // end namespace CandyTestFramework

namespace Implementation1 {

	/* we have a simple expression of the form
			expr -> function(expr...) | polynomial

		this we have the transform
			D(function(expr),sym) -> D(function,0)(expr...)D(expr[1])...
			D(polynomial,sym) -> "usual algebra"


		Thus we have
			u = sin (a b ) + c b^2 + a^3 c^2
					 ----    ---------------  2 polynomials
				----------                    a function (x) -> sin(x)
				----------------------------  a function (x,y) -> x + y
			   = Plus, Sin( Polynomial("a b") ), Polynomial("c b^2 + a^3 c^2"))


	*/

	struct SymbolMap {
		void Define(std::string const& sym, double val) {
			values_[sym] = val;
		}
		double Value(std::string const& sym)const {
			if (auto ptr = MaybeValue(sym))
				return *ptr;
			std::stringstream ss;
			ss << "don't have symbol " << sym;
			throw std::domain_error(ss.str());
		}
		double const* MaybeValue(std::string const& sym)const {
			auto iter = values_.find(sym);
			if (iter == values_.end()) {
				return nullptr;
			}
			return &iter->second;
		}
		void Report(std::ostream& ostr = std::cout)const {
			for (auto const& item : values_) {
				ostr << std::left << std::setw(4) << item.first << "=>" << item.second << "\n";
			}
		}
	private:
		std::unordered_map<std::string, double> values_;
	};

	struct StringTemplatePiece {
		virtual ~StringTemplatePiece() = default;
		virtual void Emit(SymbolMap const& S, std::ostream& out)const = 0;
	};
	struct StringTemplatePiece_String : StringTemplatePiece {
		explicit StringTemplatePiece_String(std::string const& s) :s_{ s } {}
		virtual void Emit(SymbolMap const& S, std::ostream& out)const {
			out << s_;
		}
	private:
		std::string s_;
	};
	struct StringTemplatePiece_Sym :StringTemplatePiece {
		explicit StringTemplatePiece_Sym(std::string const& sym) :sym_{ sym } {}
		virtual void Emit(SymbolMap const& S, std::ostream& out)const {
			if (auto ptr = S.MaybeValue(sym_)) {
				out << *ptr;
			}
			else {
				out << sym_;
			}
		}
	private:
		std::string sym_;
	};
	struct StringTemplate {
		template<class T, class... Args>
		void Add(Args&&... args){
			auto ptr = std::make_shared<T>(args...);
			vec_.push_back(std::static_pointer_cast<StringTemplatePiece>(ptr));
		}
		std::string Render(SymbolMap const& S)const{
			std::stringstream ss;
			for (auto const& ptr : vec_) {
				ptr->Emit(S, ss);
			}
			return ss.str();
		}
	private:
		std::vector<std::shared_ptr<StringTemplatePiece> > vec_;
	};

	struct Expr {
		virtual ~Expr() = default;
		virtual double Eval(SymbolMap const& S)const = 0;
		virtual std::shared_ptr<Expr> Diff(std::string const& sym)const = 0;
		virtual void StringRepresentation(StringTemplate&)const = 0;
		virtual bool IsZero()const {
			return false;
		}
	};

	

	struct Polynomial : Expr {

		struct Factor {
			std::string Symbol;
			int Exponent;
		};

		struct Term {
			double Multiple{ 1.0 };
			std::vector<Factor> Factors;
		};

		static std::shared_ptr<Polynomial> Make(
			std::string const& sym_0, int exp_0) {
			auto result = std::make_shared<Polynomial>();
			result->MakeTerm()
				.AddFactor(sym_0, exp_0);
			return result;
		}
		static std::shared_ptr<Polynomial> Make(
			std::string const& sym_0, int exp_0,
			std::string const& sym_1, int exp_1) {
			auto result = std::make_shared<Polynomial>();
			result->MakeTerm()
				.AddFactor(sym_0, exp_0)
				.AddFactor(sym_1, exp_1);
			return result;
		}


		struct TermProxy {
			TermProxy& AddFactor(std::string const& sym, int exp) {
				mem_->Factors.push_back(Factor{ sym, exp });
				return *this;
			}
			void Multiple(double x) {
				mem_->Multiple = x;
			}
			Term* mem_;
		};

		TermProxy MakeTerm() {
			terms_.emplace_back();
			return TermProxy{ &terms_.back() };
		}

		void Constant(double c) {
			constant_ = c;
		}

		static std::shared_ptr<Expr> MakeConstant(double c) {
			auto ptr = std::make_shared<Polynomial>();
			ptr->Constant(c);
			return ptr;
		}
		static std::shared_ptr<Expr> One() {
			return MakeConstant(1.0);
		}
		static std::shared_ptr<Expr> Zero() {
			return MakeConstant(0.0);
		}

		virtual double Eval(SymbolMap const& S)const {
			double result = constant_;
			for (auto const& term : terms_) {
				if (term.Factors.empty())
					continue;
				double sub_result = term.Multiple;
				for (auto const& factor : term.Factors) {
					sub_result *= std::pow(S.Value(factor.Symbol), static_cast<double>(factor.Exponent));
				}
				result += sub_result;
				//std::cout << "-- sub_result => " << sub_result << "\n";
			}
			return result;
		}
		virtual std::shared_ptr<Expr> Diff(std::string const& sym)const {
			// don't need the constant, but we get a new one a x + b -> a etc
			double constant_term = 0.0;
			auto result = std::make_shared<Polynomial>();
			for (auto const& term : terms_) {

				Term diff_term;
				diff_term.Multiple = term.Multiple;
				bool keep_term = false;
				// do nothing for degenerate case
				
				for(auto const& factor : term.Factors){
					if (factor.Symbol == sym){
						if (factor.Exponent != 1) {
							diff_term.Factors.push_back(Factor{ factor.Symbol, factor.Exponent - 1 });
							diff_term.Multiple *= factor.Exponent;
						}
						keep_term = true;
					}
					else {
						diff_term.Factors.push_back(factor);
					}
				}
				if (keep_term) {
					if (diff_term.Factors.empty()) {
						constant_term += diff_term.Multiple;
					} 	else {
						result->terms_.push_back(diff_term);
					}
				}
			}
			result->Constant(constant_term);
			return std::static_pointer_cast<Expr>(result);
		}
		virtual void StringRepresentation(StringTemplate& SR)const {
			for (size_t idx = 0; idx != terms_.size(); ++idx) {
				if (idx != 0)
					SR.Add<StringTemplatePiece_String>("+ ");
				if (terms_[idx].Multiple != 1.0 || terms_[idx].Factors.empty() ) {
					std::stringstream ss;
					ss << terms_[idx].Multiple;
					SR.Add<StringTemplatePiece_String>(ss.str());
				}
				for (auto const& term : terms_[idx].Factors) {
	
					SR.Add<StringTemplatePiece_Sym>(term.Symbol);
					if (term.Exponent != 1) {
						std::stringstream ss;
						ss << "^" << term.Exponent;
						SR.Add<StringTemplatePiece_String>(ss.str());
					}
					SR.Add<StringTemplatePiece_String>(" ");
				}
			}
		}
		virtual bool IsZero()const {
			for (auto const& term : terms_) {
				if (term.Factors.size() > 0 && term.Multiple != 0.0)
					return false;
			}
			return true;
		}
	private:
		double constant_{ 0 };
		std::vector<Term> terms_;
	};

	struct BinaryOperator : Expr {
		BinaryOperator(char op,
			std::shared_ptr<Expr> const& lp,
			std::shared_ptr<Expr> const& rp)
			: op_{ op }, lp_{ lp }, rp_{ rp }
		{}
		virtual double Eval(SymbolMap const& S)const {
			auto computed_lp = lp_->Eval(S);
			auto computed_rp = rp_->Eval(S);
			switch (op_) {
			case '+':
				return computed_lp + computed_rp;
			case '-':
				return computed_lp - computed_rp;
			case '*':
				return computed_lp * computed_rp;
			case '/':
				return computed_lp / computed_rp;
			default:
				throw std::domain_error("unknown op");
			}

		}
		virtual std::shared_ptr<Expr> Diff(std::string const& sym)const {
			
			auto diff_lp = lp_->Diff(sym);
			auto diff_rp = rp_->Diff(sym);

			switch (op_) {
			case '+':
			{
#if 0
				if (diff_lp->IsZero()) {
					if (diff_rp->IsZero()) {
						return std::make_shared<Polynomial>();
					}
					return diff_rp;
				}
				else {
					if (diff_rp->IsZero()) {
						return diff_lp;
					}
					return std::static_pointer_cast<Expr>(
						std::make_shared<BinaryOperator>(
							'+',
							diff_lp,
							diff_rp
							)
						);
				}
#endif
				return std::static_pointer_cast<Expr>(
					std::make_shared<BinaryOperator>(
						'+',
						diff_lp,
						diff_rp
						)
					);
			}
			case '-':
			{
				return std::static_pointer_cast<Expr>(
					std::make_shared<BinaryOperator>(
						'-',
						lp_->Diff(sym),
						rp_->Diff(sym)
						)
					);
			}
			case '*':
			{
				std::vector<std::shared_ptr<Expr> > terms;
				//if (!diff_lp->IsZero()) 
				{
					terms.push_back(std::static_pointer_cast<Expr>(
						std::make_shared<BinaryOperator>(
							'*',
							lp_->Diff(sym),
							rp_
							)
						));
				}
				//if (!diff_rp->IsZero()) 
				{
					terms.push_back(std::static_pointer_cast<Expr>(
						std::make_shared<BinaryOperator>(
							'*',
							lp_,
							rp_->Diff(sym)
							)
					));
				}
				switch (terms.size()) {
				case 0:
					return std::make_shared<Polynomial>();
				case 1:
					return terms[0];
				case 2:
					return std::make_shared<BinaryOperator>(
						'+',
						terms[0], terms[1]);
				default:
                                        ;
					//_assume(0);

				}
			}
			// f(x)^-1 = (-1)f(x)^-2 f'(x)
			case '/':
				throw std::domain_error("D for divide not impleented");
			default:
				throw std::domain_error("unknown op");
			}
		}
		virtual void StringRepresentation(StringTemplate& SR)const {
			SR.Add<StringTemplatePiece_String>("(");
			lp_->StringRepresentation(SR);
			SR.Add<StringTemplatePiece_String>(")");
			SR.Add<StringTemplatePiece_String>(std::string{ op_ });
			SR.Add<StringTemplatePiece_String>("(");
			rp_->StringRepresentation(SR);
			SR.Add<StringTemplatePiece_String>(")");
		}
	private:
		char op_;
		std::shared_ptr<Expr> lp_;
		std::shared_ptr<Expr> rp_;
	};


	void PrintDiff(std::shared_ptr<Expr> const& ptr, std::vector<std::string> const& D) {
		StringTemplate expr_st;
		ptr->StringRepresentation(expr_st);
		auto root = ptr;
		for (auto const& sym : D) {
			root = root->Diff(sym);
		}
		StringTemplate tpl;
		root->StringRepresentation(tpl);
		SymbolMap S;

		std::stringstream D_sstr;
		for (auto const& s : D) {
			D_sstr << ", " << s;
		}

		std::cout << "D[" << expr_st.Render(S) << D_sstr.str() << "] => " << tpl.Render(S) << "\n";
	}

	struct Sin : Expr {
		explicit Sin(std::shared_ptr<Expr> const& arg) :arg_{ arg } {}
		virtual double Eval(SymbolMap const& S)const {
			auto computed_param = arg_->Eval(S);
			return std::sin(computed_param);
		}
		virtual std::shared_ptr<Expr> Diff(std::string const& sym)const;
		virtual void StringRepresentation(StringTemplate& SR)const {
			SR.Add<StringTemplatePiece_String>("sin(");
			arg_->StringRepresentation(SR);
			SR.Add<StringTemplatePiece_String>(")");
		}
	private:
		std::shared_ptr<Expr> arg_;
	};
	struct Cos : Expr {
		explicit Cos(std::shared_ptr<Expr> const& arg) :arg_{ arg } {}
		virtual double Eval(SymbolMap const& S)const {
			auto computed_param = arg_->Eval(S);
			return std::cos(computed_param);
		}
		virtual std::shared_ptr<Expr> Diff(std::string const& sym)const;
		virtual void StringRepresentation(StringTemplate& SR)const {
			SR.Add<StringTemplatePiece_String>("cos(");
			arg_->StringRepresentation(SR);
			SR.Add<StringTemplatePiece_String>(")");
		}
	private:
		std::shared_ptr<Expr> arg_;
	};
	std::shared_ptr<Expr> Sin::Diff(std::string const& sym)const {
		auto arg_diff = arg_->Diff(sym);

		return std::static_pointer_cast<Expr>(
			std::make_shared<BinaryOperator>(
				'*',
				std::make_shared<Cos>(arg_),
				std::static_pointer_cast<Expr>(arg_diff)
				)
			);
	}
	std::shared_ptr<Expr> Cos::Diff(std::string const& sym)const {
		
		auto arg_diff = arg_->Diff(sym);

		auto zero = std::make_shared<Polynomial>();
		zero->MakeTerm().Multiple(0.0);


		return std::static_pointer_cast<Expr>(
			std::make_shared<BinaryOperator>(
				'-',
				zero,
				std::make_shared<BinaryOperator>(
					'*',
					std::make_shared<Sin>(arg_),
					std::static_pointer_cast<Expr>(arg_diff)
					)
				)
			);

	}



	struct Exp : Expr {
		explicit Exp(std::shared_ptr<Expr> const& arg) :arg_{ arg } {}
		virtual double Eval(SymbolMap const& S)const {
			auto computed_param = arg_->Eval(S);
			return std::exp(computed_param);
		}
		virtual std::shared_ptr<Expr> Diff(std::string const& sym)const {
			return std::make_shared<BinaryOperator>(
				'*',
				std::make_shared<Exp>(arg_),
				arg_->Diff(sym));
		}
		virtual void StringRepresentation(StringTemplate& SR)const {
			SR.Add<StringTemplatePiece_String>("exp(");
			arg_->StringRepresentation(SR);
			SR.Add<StringTemplatePiece_String>(")");
		}
	private:
		std::shared_ptr<Expr> arg_;
	};
	struct Log : Expr {
		explicit Log(std::shared_ptr<Expr> const& arg) :arg_{ arg } {}
		virtual double Eval(SymbolMap const& S)const {
			auto computed_param = arg_->Eval(S);
			return std::log(computed_param);
		}
		virtual std::shared_ptr<Expr> Diff(std::string const& sym)const {
			return std::make_shared<BinaryOperator>(
				'*',
				std::make_shared<BinaryOperator>(
					'/',
					Polynomial::One(),
					arg_
				),
				arg_->Diff(sym));
		}
		virtual void StringRepresentation(StringTemplate& SR)const {
			SR.Add<StringTemplatePiece_String>("log(");
			arg_->StringRepresentation(SR);
			SR.Add<StringTemplatePiece_String>(")");
		}
	private:
		std::shared_ptr<Expr> arg_;
	};


	

	






	

	void TestDriver1() {
		auto poly = std::make_shared<Polynomial>();
		auto t0 = poly->MakeTerm();
		t0.AddFactor("a", 2);
		t0.AddFactor("b", 3);

		auto expr = std::static_pointer_cast<Expr>(poly);

		PrintDiff(expr, { "a" });
		PrintDiff(expr, { "a", "a" });
		PrintDiff(expr, { "a", "a", "a" });
		PrintDiff(expr, { "b" });

		SymbolMap S;
		S.Define("a", 7);
		S.Define("b", 3);

		std::cout << "value = " << expr->Eval(S) << "\n";
		
		auto X = std::make_shared<Polynomial>();
		X->MakeTerm().AddFactor("x", 1);

		auto Y = std::make_shared<Polynomial>();
		Y->MakeTerm().AddFactor("y", 2);

		auto X_plus_Y = std::static_pointer_cast<Expr>(
			std::make_shared<BinaryOperator>('*',
				std::static_pointer_cast<Expr>(X),
				std::static_pointer_cast<Expr>(Y)));


		PrintDiff(X_plus_Y, { "x" });
		PrintDiff(X_plus_Y, { "y" });
		
		auto AB = std::make_shared<Polynomial>();
		AB->MakeTerm().AddFactor("a", 1).AddFactor("b", 1);
		auto CBB = std::make_shared<Polynomial>();
		CBB->MakeTerm().AddFactor("c", 1).AddFactor("b", 2);
		auto AAACC = std::make_shared<Polynomial>();
		AAACC->MakeTerm().AddFactor("a", 3)
			.AddFactor("c", 2)
			;

#if 1
		auto U = std::make_shared<BinaryOperator>(
			'+',
			std::make_shared<BinaryOperator>(
				'+',
				std::make_shared<Sin>(AB),
				CBB
			),
			AAACC);
#else
		auto U = 
			AAACC;
#endif


		PrintDiff(U, { "a" });
		PrintDiff(U, { "b" });
		PrintDiff(U, { "c" });

		auto test_U = [&](auto a, auto b, auto c) {
			auto analytic_u       = std::sin(a*b) + c * b * b + a*a*a*c*c;
			auto analytic_d_u_d_a = b*std::cos(a*b)  + 3*a*a*c*c;

			SymbolMap sm;
			sm.Define("a", a);
			sm.Define("b", b);
			sm.Define("c", c);
			auto computed_u = U->Eval(sm);
			

			std::cout << "analytic_u: " << computed_u << "\n";
			std::cout << "computed_u: " << computed_u << "\n";
			std::cout << "analytic_d_u_d_a: " << analytic_d_u_d_a << "\n";
			auto computed_d_u_d_a = U->Diff("a")->Eval(sm);
			std::cout << "computed_d_u_d_a: " << computed_d_u_d_a << "\n";

			std::cout << "b*std::cos(a*b) = " << b*std::cos(a*b) << "\n";
			std::cout << "3*a*a*c*c = " << 3 * a*a*c*c << "\n";
		};

		auto a = 0.1;
		auto b = 0.1;
		auto c = 3.0;
		test_U(a, b, c);

	}





	struct UnitSimple : CandyTestFramework::Unit {
		UnitSimple() : CandyTestFramework::Unit("UnitSimple", SOURCE_INFO()) {}
		virtual void RunUnit(CandyTestFramework::CandyTestContextUnit* unit)const override {
		

			auto X = std::make_shared<Polynomial>();
			X->MakeTerm().AddFactor("x", 3);

			auto Y = std::make_shared<Polynomial>();
			Y->MakeTerm().AddFactor("y", 2);

			auto X_times_Y = std::static_pointer_cast<Expr>(
				std::make_shared<BinaryOperator>('*',
					std::static_pointer_cast<Expr>(X),
					std::static_pointer_cast<Expr>(Y)));

			auto X_plus_Y = std::static_pointer_cast<Expr>(
				std::make_shared<BinaryOperator>('+',
					std::static_pointer_cast<Expr>(X),
					std::static_pointer_cast<Expr>(Y)));

			auto X_plus_zero = std::static_pointer_cast<Expr>(
				std::make_shared<BinaryOperator>('+',
					std::static_pointer_cast<Expr>(X),
					std::make_shared<Polynomial>()));



			auto x = 2.0;
			auto y = 3.0;
			SymbolMap sm;
			sm.Define("x", x);
			sm.Define("y", y);

			unit->CheckEqWithExpr(x*x*x, X->Eval(sm));
			unit->CheckEqWithExpr(3 * x*x, X->Diff("x")->Eval(sm));
			unit->CheckEqWithExpr(6 * x, X->Diff("x")->Diff("x")->Eval(sm));
			unit->CheckEqWithExpr(6.0, X->Diff("x")->Diff("x")->Diff("x")->Eval(sm));

			unit->CheckEqWithExpr(x*x*x, X_plus_zero->Eval(sm));
			unit->CheckEqWithExpr(3 * x*x, X_plus_zero->Diff("x")->Eval(sm));
			unit->CheckEqWithExpr(6 * x, X_plus_zero->Diff("x")->Diff("x")->Eval(sm));
			unit->CheckEqWithExpr(6.0, X_plus_zero->Diff("x")->Diff("x")->Diff("x")->Eval(sm));


			unit->CheckEqWithExpr(x*x*x + y*y  , X_plus_Y->Eval(sm));
			unit->CheckEqWithExpr(3 * x*x , X_plus_Y->Diff("x")->Eval(sm));
			unit->CheckEqWithExpr( 2 * y, X_plus_Y->Diff("y")->Eval(sm));

			unit->CheckEqWithExpr(x*x*x*y*y, X_times_Y->Eval(sm));
			unit->CheckEqWithExpr(3 * x*x*y*y, X_times_Y->Diff("x")->Eval(sm));
			unit->CheckEqWithExpr(2 * x*x*x*y, X_times_Y->Diff("y")->Eval(sm));

			auto Log_X = std::make_shared<Log>(X);
			auto Exp_X = std::make_shared<Exp>(X);

			unit->CheckEqWithExpr(std::log(x*x*x), Log_X->Eval(sm));
			unit->CheckEqWithExpr((1 / (x*x*x)) * 3 * x*x, Log_X->Diff("x")->Eval(sm));

			unit->CheckEqWithExpr(std::exp(x*x*x), Exp_X->Eval(sm));
			unit->CheckEqWithExpr(std::exp(x*x*x) * 3 * x*x, Exp_X->Diff("x")->Eval(sm));


		}

	};
	static UnitSimple RegUnitSimple;

	struct UnitSimpleB : CandyTestFramework::Unit {
		UnitSimpleB() : CandyTestFramework::Unit("UnitSimpleB", SOURCE_INFO()) {}
		virtual void RunUnit(CandyTestFramework::CandyTestContextUnit* unit)const override {


			auto AB = std::make_shared<Polynomial>();
			AB->MakeTerm().AddFactor("a", 1).AddFactor("b", 1);
			auto CBB = std::make_shared<Polynomial>();
			CBB->MakeTerm().AddFactor("c", 1).AddFactor("b", 2);
			auto AAACC = std::make_shared<Polynomial>();
			AAACC->MakeTerm().AddFactor("a", 3).AddFactor("c", 2);

			auto Sin_AB = std::make_shared<Sin>(AB);


			auto a = 0.1;
			auto b = 0.1;
			auto c = 3.0;

			SymbolMap sm;
			sm.Define("a", a);
			sm.Define("b", b);
			sm.Define("c", c);

			unit->CheckEqWithExpr(std::sin(a*b), Sin_AB->Eval(sm));
			unit->CheckEqWithExpr(b*std::cos(a*b), Sin_AB->Diff("a")->Eval(sm));
			unit->CheckEqWithExpr(a*std::cos(a*b), Sin_AB->Diff("b")->Eval(sm));

			unit->CheckEqWithExpr(a*a*a*c*c, AAACC->Eval(sm));
			unit->CheckEqWithExpr(3 * a*a*c*c, AAACC->Diff("a")->Eval(sm));

			unit->CheckEqWithExpr(0.0, AAACC->Diff("x")->Eval(sm));

			auto U = std::make_shared<BinaryOperator>(
				'+',
				std::make_shared<BinaryOperator>(
					'+',
					std::make_shared<Sin>(AB),
					CBB
					),
				AAACC);

			unit->CheckEqWithExpr(b*std::cos(a*b) + 3 * a*a*c*c, U->Diff("a")->Eval(sm));
			unit->CheckEqWithExpr(a*std::cos(a*b) + 2 * c*b, U->Diff("b")->Eval(sm));
			unit->CheckEqWithExpr(b*b + 2 * a*a*a*c, U->Diff("c")->Eval(sm));

			
			// increase the information
			auto u = U->Eval(sm);
			sm.Define("u", u);
		
			// u^2-1
			auto UU = std::make_shared<Polynomial>();
			UU->MakeTerm().AddFactor("u", 2);
			UU->Constant(-1.0);
			auto AA = std::make_shared<Polynomial>();
			AA->MakeTerm().AddFactor("a", 2);

			auto V = std::make_shared<BinaryOperator>(
				'+',
				std::make_shared<Exp>(UU),
				AA);

			

			unit->CheckEqWithExpr(std::exp(u*u - 1) + a*a, V->Eval(sm));
			unit->CheckEqWithExpr(2*u*std::exp(u*u - 1) , V->Diff("u")->Eval(sm));
			unit->CheckEqWithExpr(2*a, V->Diff("a")->Eval(sm));
			unit->CheckEqWithExpr(0, V->Diff("b")->Eval(sm));
			unit->CheckEqWithExpr(0, V->Diff("c")->Eval(sm));

			// increase the information
			auto v = V->Eval(sm);
			sm.Define("v", v);

			auto VV_p1 = std::make_shared<Polynomial>();
			VV_p1->MakeTerm().AddFactor("v", 2);
			VV_p1->Constant(1.0);

			auto CC_m1 = std::make_shared<Polynomial>();
			CC_m1->MakeTerm().AddFactor("c", 2);
			CC_m1->Constant(-1.0);

			auto W = std::make_shared<BinaryOperator>(
				'+',
				std::make_shared<Log>(VV_p1),
				std::make_shared<Cos>(CC_m1));

			unit->CheckEqWithExpr(std::log(v*v + 1) , std::make_shared<Log>(VV_p1)->Eval(sm));
			unit->CheckEqWithExpr(std::cos(c*c - 1), std::make_shared<Cos>(CC_m1)->Eval(sm));
			unit->CheckEqWithExpr(std::log(v*v + 1) + std::cos(c*c - 1), W->Eval(sm));

			// increase the information
			auto w = W->Eval(sm);
			auto w0 = 0.25;
			sm.Define("w", w);
			sm.Define("w0", w0);

			auto W_ = std::make_shared<Polynomial>();
			W_->MakeTerm().AddFactor("w",1);
			auto W0 = std::make_shared<Polynomial>();
			W0->MakeTerm().AddFactor("w0",1);
			auto W_minus_W0 = std::make_shared<BinaryOperator>(
				'-',
				Polynomial::Make("w", 1), 
				Polynomial::Make("w0", 1));
			auto F = std::make_shared<BinaryOperator>(
				'*',
				W_minus_W0,
				W_minus_W0
				);

			unit->CheckEqWithExpr(std::pow(w- w0, 2.0), F->Eval(sm));

			

		}
	};
	static UnitSimpleB RegUnitSimpleB;

	struct UnitSimpleC : CandyTestFramework::Unit {
		UnitSimpleC() : CandyTestFramework::Unit("UnitSimpleC", SOURCE_INFO()) {}
		virtual void RunUnit(CandyTestFramework::CandyTestContextUnit* unit)const override {

			auto a = 0.1;
			auto b = 0.1;
			auto c = 3.0;
	
			SymbolMap sm;
			sm.Define("a", a);
			sm.Define("b", b);
			sm.Define("c", c);

			auto U = std::make_shared<BinaryOperator>(
				'+',
				std::make_shared<BinaryOperator>(
					'+',
					std::make_shared<Sin>(Polynomial::Make("a", 1, "b", 1)),
					Polynomial::Make("c", 1, "b", 2)
				),
				Polynomial::Make("a", 3, "c", 2)
			);

			
			

			unit->CheckEqWithExpr(b*std::cos(a*b) + 3 * a*a*c*c, U->Diff("a")->Eval(sm));
			unit->CheckEqWithExpr(a*std::cos(a*b) + 2 * c*b, U->Diff("b")->Eval(sm));
			unit->CheckEqWithExpr(b*b + 2 * a*a*a*c, U->Diff("c")->Eval(sm));


			// increase the information
			auto u = U->Eval(sm);
			sm.Define("u", u);

			auto V = std::make_shared<BinaryOperator>(
				'+',
				std::make_shared<Exp>(
					std::make_shared<BinaryOperator>(
						'-',
						Polynomial::Make("u",2),
						Polynomial::MakeConstant(1)
					)
				),
				Polynomial::Make("a", 2)
			);

			unit->CheckEqWithExpr(std::exp(u*u - 1) + a*a, V->Eval(sm));
			unit->CheckEqWithExpr(2 * u*std::exp(u*u - 1), V->Diff("u")->Eval(sm));
			unit->CheckEqWithExpr(2 * a, V->Diff("a")->Eval(sm));
			unit->CheckEqWithExpr(0, V->Diff("b")->Eval(sm));
			unit->CheckEqWithExpr(0, V->Diff("c")->Eval(sm));

			// increase the information
			auto v = V->Eval(sm);
			sm.Define("v", v);

			auto W = std::make_shared<BinaryOperator>(
				'+',
				std::make_shared<Log>(
					std::make_shared<BinaryOperator>(
						'+',
						Polynomial::Make("v", 2),
						Polynomial::MakeConstant(1.0)
						)
				),
				std::make_shared<Cos>(
					std::make_shared<BinaryOperator>(
						'-',
						Polynomial::Make("c", 2),
						Polynomial::MakeConstant(1.0)
						)
					)
				);

			unit->CheckEqWithExpr(std::log(v*v + 1) + std::cos(c*c - 1), W->Eval(sm));

			// increase the information
			auto w = W->Eval(sm);
			auto constexpr w0 = 2.0;
			sm.Define("w", w);
			sm.Define("w0", w0);

			auto F = std::make_shared<BinaryOperator>(
				'*',
				std::make_shared<BinaryOperator>(
					'-',
					Polynomial::Make("w", 1),
					Polynomial::Make("w0", 1)),
				std::make_shared<BinaryOperator>(
					'-',
					Polynomial::Make("w", 1),
					Polynomial::Make("w0", 1))
				);

			unit->CheckEqWithExpr(std::pow(w - w0, 2.0), F->Eval(sm));
			unit->CheckEqWithExpr( 2*(w - w0), F->Diff("w")->Eval(sm));

			SymbolMap information;
			information.Define("a", a);
			information.Define("b", b);
			information.Define("c", c);
			information.Define("D[a]", 1);
			information.Define("D[b]", 0);
			information.Define("D[c]", 0);
			information.Define("w0", w0);
			information.Define("u", U->Eval(information));
			information.Define("v", V->Eval(information));
			information.Define("w", W->Eval(information));
			

			auto matrix_comp_0 = Matrix_ty(4, 3);
			matrix_comp_0.fill(0.0);
			matrix_comp_0(0, 0) = 1;
			matrix_comp_0(1, 1) = 1;
			matrix_comp_0(2, 2) = 1;
			matrix_comp_0(3, 0) = U->Diff("a")->Eval(information);
			matrix_comp_0(3, 1) = U->Diff("b")->Eval(information);
			matrix_comp_0(3, 2) = U->Diff("c")->Eval(information);
			std::cout << matrix_comp_0 << "\n";

			auto input_0 = Vector_ty(3);
			input_0(0) = information.Value("D[a]");
			input_0(1) = information.Value("D[b]");
			input_0(2) = information.Value("D[c]");


			auto comp_0 = matrix_comp_0 * input_0;
			std::cout << "comp_0\n" << comp_0 << "\n";

			information.Define("D[u]", comp_0(3));

			
			auto matrix_comp_1 = Matrix_ty(1, 2);
			matrix_comp_1(0) = V->Diff("a")->Eval(information);
			matrix_comp_1(1) = V->Diff("u")->Eval(information);

			std::cout << matrix_comp_1 << "\n";

			auto input_1 = Vector_ty(2);
			input_1(0) = information.Value("D[a]");
			input_1(1) = information.Value("D[u]");

			auto comp_1 = matrix_comp_1 * input_1;
			std::cout << "comp_1\n" << comp_1 << "\n";

			information.Define("D[v]", comp_1(0));




			auto matrix_comp_2 = Matrix_ty(1, 2);
			matrix_comp_2(0) = W->Diff("v")->Eval(information);
			matrix_comp_2(1) = W->Diff("c")->Eval(information);

			std::cout << "matrix_comp_2\n" << matrix_comp_2 << "\n";

			auto input_2 = Vector_ty(2);
			input_2(0) = information.Value("D[v]");
			input_2(1) = information.Value("D[c]");

			auto comp_2 = matrix_comp_2 * input_2;
			std::cout << "comp_2\n" << comp_2 << "\n";

			information.Define("D[w]", comp_2(0));


			auto matrix_comp_3 = Matrix_ty(1, 1);
			matrix_comp_3(0, 0) = F->Diff("w")->Eval(information);

			std::cout << "matrix_comp_3\n" << matrix_comp_3 << "\n";

			auto input_3 = Vector_ty(1);
			input_3(0) = information.Value("D[w]");

			auto comp_3 = matrix_comp_3 * input_3;
			std::cout << "comp_3\n" << comp_3 << "\n";



			std::cout << "F(X) = " << F->Eval(sm) << "\n";
			std::cout << "df/da = " << comp_3(0) << "\n";


			auto d = Vector_ty(7);
			d(0) = information.Value("D[a]");
			d(1) = information.Value("D[b]");
			d(2) = information.Value("D[c]");
			d(3) = information.Value("D[u]");
			d(4) = information.Value("D[v]");
			d(5) = information.Value("D[w]");
			d(6) = comp_3(0);

			std::cout << "d=\n" << d << "\n";

			information.Report();

			


		}
	};
	static UnitSimpleC RegUnitSimpleC;

	struct UnitSimpleD : CandyTestFramework::Unit {
		UnitSimpleD() : CandyTestFramework::Unit("UnitSimpleD", SOURCE_INFO()) {}
		virtual void RunUnit(CandyTestFramework::CandyTestContextUnit* unit)const override {

			auto x = 3.0;

			SymbolMap sm;
			sm.Define("x", x);

			unit->CheckEqWithExpr(x*x*x, Polynomial::Make("x", 3)->Eval(sm));
			unit->CheckEqWithExpr(3*x*x, Polynomial::Make("x", 3)->Diff("x")->Eval(sm));
			unit->CheckEqWithExpr(6*x, Polynomial::Make("x", 3)->Diff("x")->Diff("x")->Eval(sm));
			unit->CheckEqWithExpr(6 , Polynomial::Make("x", 3)->Diff("x")->Diff("x")->Diff("x")->Eval(sm));
			unit->CheckEqWithExpr(0, Polynomial::Make("x", 3)->Diff("x")->Diff("x")->Diff("x")->Diff("x")->Eval(sm));

			unit->CheckEqWithExpr(3*x*x, std::make_shared<BinaryOperator>('*',
				Polynomial::MakeConstant(3.0),
				Polynomial::Make("x", 2)
				)->Eval(sm));

			unit->CheckEqWithExpr(6 , std::make_shared<BinaryOperator>('*',
				Polynomial::MakeConstant(6.0),
				Polynomial::Make("x", 1)
				)->Diff("x")->Eval(sm));

			

		}
	};
	static UnitSimpleD RegUnitSimpleD;

	struct UnitSimpleE : CandyTestFramework::Unit {
		UnitSimpleE() : CandyTestFramework::Unit("UnitSimpleE", SOURCE_INFO()) {}
		virtual void RunUnit(CandyTestFramework::CandyTestContextUnit* unit)const override {
			
			auto U = std::make_shared<BinaryOperator>(
				'+',
				std::make_shared<BinaryOperator>(
					'+',
					std::make_shared<Sin>(Polynomial::Make("a", 1, "b", 1)),
					Polynomial::Make("c", 1, "b", 2)
					),
				Polynomial::Make("a", 3, "c", 2)
				);

			auto V = std::make_shared<BinaryOperator>(
				'+',
				std::make_shared<Exp>(
					std::make_shared<BinaryOperator>(
						'-',
						Polynomial::Make("u", 2),
						Polynomial::MakeConstant(1)
						)
					),
				Polynomial::Make("a", 2)
				);

			auto W = std::make_shared<BinaryOperator>(
				'+',
				std::make_shared<Log>(
					std::make_shared<BinaryOperator>(
						'+',
						Polynomial::Make("v", 2),
						Polynomial::MakeConstant(1.0)
						)
					),
				std::make_shared<Cos>(
					std::make_shared<BinaryOperator>(
						'-',
						Polynomial::Make("c", 2),
						Polynomial::MakeConstant(1.0)
						)
					)
				);

			auto F = std::make_shared<BinaryOperator>(
				'*',
				std::make_shared<BinaryOperator>(
					'-',
					Polynomial::Make("w", 1),
					Polynomial::Make("w0", 1)),
				std::make_shared<BinaryOperator>(
					'-',
					Polynomial::Make("w", 1),
					Polynomial::Make("w0", 1))
				);

			struct State {
				std::vector<std::string> Allocation;
				SymbolMap Symbols;
			};
			struct Operator {
				virtual ~Operator() = default;
				virtual std::shared_ptr<State> ExecuteNext(State const& state)const = 0;
			};
			struct Step : Operator {
				Step(std::string const& sym, std::shared_ptr<Expr> expr)
					:sym_{ sym }, expr_ {expr}
				{}
				virtual std::shared_ptr<State> ExecuteNext(State const& state)const
				{
					/*
						first we make the matrix
					*/
					size_t n = state.Allocation.size();
					auto inputs = Vector_ty(n);
					auto diffs = Vector_ty(n);
					for (size_t idx = 0; idx != state.Allocation.size(); ++idx) {
						auto const& sym = state.Allocation[idx];
						auto d_sym = "D[" + sym + "]";
						inputs(idx) = state.Symbols.Value(d_sym);
						diffs(idx) = expr_->Diff(sym)->Eval(state.Symbols);
					}
					
					auto d = diffs.dot(inputs);

					enum { Debug = 0 };
					if (Debug)
					{
						std::cout << "==== " << sym_ << " D ====\n" << inputs << "\n"
							<< "==== " << sym_ << " M ====\n" << diffs << "\n"
							<< "==== " << sym_ << " result => " << d << "\n";
					}

					

					auto next = std::make_shared<State>(state);
					next->Allocation.push_back(sym_);
					next->Symbols.Define(sym_, expr_->Eval(next->Symbols));
					next->Symbols.Define("D[" + sym_ + "]", d);
					return next;
				}
			private:
				std::string sym_;
				std::shared_ptr<Expr> expr_;
			};
			
			auto a = 0.1;
			auto b = 0.1;
			auto c = 3.0;

			// parameters
			auto constexpr w0 = 2.0;

			auto state0 = std::make_shared<State>();
			state0->Allocation.push_back("a");
			state0->Allocation.push_back("b");
			state0->Allocation.push_back("c");

			state0->Symbols.Define("a", a);
			state0->Symbols.Define("b", b);
			state0->Symbols.Define("c", c);
			state0->Symbols.Define("w0", w0);
			state0->Symbols.Define("D[a]", 1);
			state0->Symbols.Define("D[b]", 0);
			state0->Symbols.Define("D[c]", 0);

			std::vector<std::shared_ptr<Operator> > ops;
			ops.push_back(std::make_shared<Step>("u", U));
			ops.push_back(std::make_shared<Step>("v", V));
			ops.push_back(std::make_shared<Step>("w", W));
			ops.push_back(std::make_shared<Step>("f", F));

			std::vector<std::shared_ptr<State> > filtration;
			filtration.push_back(state0);

			for (auto const& op : ops) {
				auto next = op->ExecuteNext(*filtration.back());
				filtration.push_back(next);
			}

			std::cout << "from machine => " << filtration.back()->Symbols.Value("D[f]") << "\n";

		}
	};
	static UnitSimpleE RegUnitSimpleE;
	
	
			
	 
} // end namespace Implementation1

int main() {
	Implementation0::TestDriver();
	Implementation1::TestDriver1();
	CandyTestFramework::Unit::RunAll(CandyTestFramework::DefaultContext());

	std::cin.get();
}
