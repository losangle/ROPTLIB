/*
This file is the driver for problem defined by Matlab functions.
The Matlab driver "DriverOPT.m" creates function handles of a cost function and
parameters for both solvers and manifolds, which are passing to the
binary file generated by this C++ driver.

---- WH
*/

#ifndef DRIVERMEXPROB_H
#define DRIVERMEXPROB_H


#include <iostream>
#include "Others/randgen.h"
#include "Manifolds/Manifold.h"
#include "Problems/Problem.h"
#include "Problems/SphereTxRQ/SphereTxRQ.h"
#include "Solvers/SolversLS.h"
#include <ctime>

#include "Manifolds/Euclidean/Euclidean.h"
#include "Manifolds/Euclidean/EucVariable.h"
#include "Manifolds/Euclidean/EucVector.h"

#include "Manifolds/Stiefel/StieVector.h"
#include "Manifolds/Stiefel/StieVariable.h"
#include "Manifolds/Stiefel/Stiefel.h"

#include "Solvers/RSD.h"
#include "Solvers/RNewton.h"
#include "Solvers/RCG.h"
#include "Solvers/RBroydenFamily.h"
#include "Solvers/RWRBFGS.h"
#include "Solvers/RBFGS.h"
#include "Solvers/LRBFGS.h"
#include "Solvers/RBFGSLPSub.h"
#include "Solvers/LRBFGSLPSub.h"
#include "Solvers/RGS.h"

#include "Solvers/SolversTR.h"
#include "Solvers/RTRSD.h"
#include "Solvers/RTRNewton.h"
#include "Solvers/RTRSR1.h"
#include "Solvers/LRTRSR1.h"

#include "Others/def.h"

#include "Manifolds/Sphere/Sphere.h"
#include "Manifolds/Sphere/SphereVariable.h"
#include "Manifolds/Sphere/SphereVector.h"

#include "Manifolds/Oblique/Oblique.h"
#include "Manifolds/Oblique/ObliqueVariable.h"
#include "Manifolds/Oblique/ObliqueVector.h"

#include "Manifolds/LowRank/LowRank.h"
#include "Manifolds/LowRank/LowRankVariable.h"
#include "Manifolds/LowRank/LowRankVector.h"

#include "Manifolds/OrthGroup/OrthGroup.h"
#include "Manifolds/OrthGroup/OrthGroupVariable.h"
#include "Manifolds/OrthGroup/OrthGroupVector.h"

#include "Manifolds/SPDManifold/SPDManifold.h"
#include "Manifolds/SPDManifold/SPDVariable.h"
#include "Manifolds/SPDManifold/SPDVector.h"

#include "Manifolds/L2Sphere/L2Sphere.h"
#include "Manifolds/L2Sphere/L2SphereVariable.h"
#include "Manifolds/L2Sphere/L2SphereVector.h"

#include "Manifolds/CpxNStQOrth/CpxNStQOrth.h"
#include "Manifolds/CpxNStQOrth/CSOVariable.h"
#include "Manifolds/CpxNStQOrth/CSOVector.h"

#include "Manifolds/Grassmann/Grassmann.h"
#include "Manifolds/Grassmann/GrassVariable.h"
#include "Manifolds/Grassmann/GrassVector.h"

#include "Manifolds/EucPositive/EucPositive.h"
#include "Manifolds/EucPositive/EucPosVariable.h"
#include "Manifolds/EucPositive/EucPosVector.h"

#include "Manifolds/PreShapeCurves/PreShapeCurves.h"
#include "Manifolds/PreShapeCurves/PSCVariable.h"
#include "Manifolds/PreShapeCurves/PSCVector.h"

#include "Manifolds/SPDTensor/SPDTensor.h"
#include "Manifolds/SPDTensor/SPDTVariable.h"
#include "Manifolds/SPDTensor/SPDTVector.h"

#include "Manifolds/SphereTx/SphereTx.h"

#include "Manifolds/ProductElement.h"
#include "Manifolds/ProductManifold.h"

#ifdef MATLAB_MEX_FILE

#include "Problems/mexProblem.h"

using namespace ROPTLIB;

/*This function checks the number and formats of input parameters.
nlhs: the number of output in mxArray format
plhs: the output objects in mxArray format
nrhs: the number of input in mxArray format
prhs: the input objects in mxArray format */
void DriverMexProb(int &nlhs, mxArray ** &plhs, int &nrhs, const mxArray ** &prhs);

/*This file creates a C++ solver based on the input parameters and run the solver to obtain a solution.*/
void ParseSolverParamsAndOptimizing(const mxArray *SolverParams, Problem *Prob, Variable *initialX, Variable *soln, mxArray ** &plhs);

/*This file creates all components of the product manifolds. Note that the cost function is always
defined on a product manifolds.*/
bool ParseManiParams(const mxArray *ManiParams, Manifold **&manifolds, Element **&elements,
	integer *&powsinterval, integer &numoftype, integer &numoftotal);

/*Create a manifold based on the parameters.*/
Manifold *GetAManifold(const char *name, integer n, integer m, integer p = 1);

/*Create an element based on the parameters. Note that the element is a component of the initial iterate.*/
Element *GetAnElement(const char *name, integer n, integer m, integer p = 1);

namespace RMEX{
	mxArray *isstopped = nullptr;
	/*This function defines the stopping criterion that may be used in the C++ solver*/
	bool mexInnerStop(Variable *x, Vector *gf, double f, double ngf, double ngf0, const Problem *prob, const Solvers *solver)
	{
		mxArray *Xmx, *gfmx, *fmx, *ngfmx, *ngf0mx;
		mexProblem::ObtainMxArrayFromElement(Xmx, x);
		mexProblem::ObtainMxArrayFromElement(gfmx, gf);
		fmx = mxCreateDoubleScalar(f);
		ngfmx = mxCreateDoubleScalar(ngf);
		ngf0mx = mxCreateDoubleScalar(ngf0);

		mxArray *lhs[1], *rhs[6];
		rhs[0] = const_cast<mxArray *> (isstopped);
		rhs[1] = const_cast<mxArray *> (Xmx);
		rhs[2] = const_cast<mxArray *> (gfmx);
		rhs[3] = const_cast<mxArray *> (fmx);
		rhs[4] = const_cast<mxArray *> (ngfmx);
		rhs[5] = const_cast<mxArray *> (ngf0mx);
		mexCallMATLAB(1, lhs, 6, rhs, "feval");
		double result = mxGetScalar(lhs[0]);
		mxDestroyArray(Xmx);
		mxDestroyArray(gfmx);
		mxDestroyArray(fmx);
		mxDestroyArray(ngfmx);
		mxDestroyArray(ngf0mx);
		mxDestroyArray(lhs[0]);
		return (result != 0);
	};
	mxArray *LinesearchInput = nullptr;
	/*This function defines the line search algorithm that may be used in the C++ solver*/
	double mexLinesearchInput(integer iter, Variable *x1, Vector *eta1, double initialstepsize, double initialslope, const Problem *prob, const Solvers *solver)
	{
		mxArray *Xmx, *eta1mx, *tmx, *smx, *imx;
		mexProblem::ObtainMxArrayFromElement(Xmx, x1);
		mexProblem::ObtainMxArrayFromElement(eta1mx, eta1);
		tmx = mxCreateDoubleScalar(initialstepsize);
		smx = mxCreateDoubleScalar(initialslope);
		imx = mxCreateDoubleScalar(iter);


		mxArray *lhs[1], *rhs[6];
		rhs[0] = const_cast<mxArray *> (LinesearchInput);
		rhs[1] = const_cast<mxArray *> (Xmx);
		rhs[2] = const_cast<mxArray *> (eta1mx);
		rhs[3] = const_cast<mxArray *> (tmx);
		rhs[4] = const_cast<mxArray *> (smx);
		rhs[5] = const_cast<mxArray *> (imx);
		mexCallMATLAB(1, lhs, 6, rhs, "feval");
		double result = mxGetScalar(lhs[0]);
		mxDestroyArray(Xmx);
		mxDestroyArray(eta1mx);
		mxDestroyArray(tmx);
		mxDestroyArray(smx);
		mxDestroyArray(imx);
		mxDestroyArray(lhs[0]);
		return result;
	}
};

void ParseSolverParamsAndOptimizing(const mxArray *SolverParams, Problem *Prob, Variable *initialX, Variable *soln, mxArray **&plhs)
{
	integer nfields = mxGetNumberOfFields(SolverParams);
	const char *name;
	mxArray *tmp;
	double value;
	PARAMSMAP params;
	std::string key;
	for (integer i = 0; i < nfields; i++)
	{
		name = mxGetFieldNameByNumber(SolverParams, i);
		tmp = mxGetFieldByNumber(SolverParams, 0, i);
		value = mxGetScalar(tmp);
		key.assign(name);
		params.insert(std::pair<std::string, double>(key, value));
	}

	tmp = mexProblem::GetFieldbyName(SolverParams, 0, "method");
	if (tmp == nullptr)
	{
		mexErrMsgTxt("A method must be specified.");
	}

	char methodname[30] = "";
	mxGetString(tmp, methodname, 30);
	std::string stdmethodname = methodname;
	Solvers *solver;
	if (stdmethodname == "RSD")
	{
		solver = new RSD(Prob, initialX, soln);
	}
	else
	if (stdmethodname == "RNewton")
	{
		solver = new RNewton(Prob, initialX, soln);
	}
	else
	if (stdmethodname == "RCG")
	{
		solver = new RCG(Prob, initialX, soln);
	}
	else
	if (stdmethodname == "RBroydenFamily")
	{
		solver = new RBroydenFamily(Prob, initialX, nullptr, soln);
	}
	else
	if (stdmethodname == "RWRBFGS")
	{
		solver = new RWRBFGS(Prob, initialX, nullptr, soln);
	}
	else
	if (stdmethodname == "RBFGS")
	{
		solver = new RBFGS(Prob, initialX, nullptr, soln);
	}
	else
	if (stdmethodname == "RBFGSLPSub")
	{
		solver = new RBFGSLPSub(Prob, initialX, nullptr, soln);
	}
	else
	if (stdmethodname == "LRBFGSLPSub")
	{
		solver = new LRBFGSLPSub(Prob, initialX, soln);
	}
	else
	if (stdmethodname == "RGS")
	{
		solver = new RGS(Prob, initialX, soln);
	}
	else
	if (stdmethodname == "LRBFGS")
	{
		solver = new LRBFGS(Prob, initialX, soln);
	}
	else
	if (stdmethodname == "RTRSD")
	{
		solver = new RTRSD(Prob, initialX, soln);
	}
	else
	if (stdmethodname == "RTRNewton")
	{
		solver = new RTRNewton(Prob, initialX, soln);
	}
	else
	if (stdmethodname == "RTRSR1")
	{
		solver = new RTRSR1(Prob, initialX, nullptr, soln);
	}
	else
	if (stdmethodname == "LRTRSR1")
	{
		solver = new LRTRSR1(Prob, initialX, soln);
	}
	else
	{
		printf("Warning: Unrecognized solver: %s. Use LRBFGS instead!\n", stdmethodname.c_str());
		solver = new LRBFGS(Prob, initialX, soln);
	}
	solver->SetParams(params);


	RMEX::isstopped = mexProblem::GetFieldbyName(SolverParams, 0, "IsStopped");
	if (RMEX::isstopped != nullptr)
	{
		solver->StopPtr = &RMEX::mexInnerStop;
	}

	RMEX::LinesearchInput = mexProblem::GetFieldbyName(SolverParams, 0, "LinesearchInput");
	if (RMEX::LinesearchInput != nullptr)
	{
		SolversLS *solverLS = dynamic_cast<SolversLS *> (solver);
		if (solverLS != nullptr)
			solverLS->LinesearchInput = &RMEX::mexLinesearchInput;
	}

	tmp = mexProblem::GetFieldbyName(SolverParams, 0, "IsCheckParams");
	if (tmp != nullptr)
	{
		if (fabs(mxGetScalar(tmp)) > std::numeric_limits<double>::epsilon()) // if the value is nonzero
		{
			solver->CheckParams();
		}
	}
	solver->Run();
	mexProblem::ObtainMxArrayFromElement(plhs[0], solver->GetXopt());
	plhs[1] = mxCreateDoubleScalar(static_cast<double> (solver->Getfinalfun()));
	plhs[2] = mxCreateDoubleScalar(static_cast<double> (solver->Getnormgf()));
	plhs[3] = mxCreateDoubleScalar(static_cast<double> (solver->Getnormgfgf0()));
	plhs[4] = mxCreateDoubleScalar(static_cast<double> (solver->GetIter()));
	plhs[5] = mxCreateDoubleScalar(static_cast<double> (solver->Getnf()));
	plhs[6] = mxCreateDoubleScalar(static_cast<double> (solver->Getng()));
	plhs[7] = mxCreateDoubleScalar(static_cast<double> (solver->GetnR()));
	plhs[8] = mxCreateDoubleScalar(static_cast<double> (solver->GetnV()));
	plhs[9] = mxCreateDoubleScalar(static_cast<double> (solver->GetnVp()));
	plhs[10] = mxCreateDoubleScalar(static_cast<double> (solver->GetnH()));
	plhs[11] = mxCreateDoubleScalar(static_cast<double> (solver->GetComTime()));
	integer lengthSeries = solver->GetlengthSeries();
	plhs[12] = mxCreateDoubleMatrix(lengthSeries, 1, mxREAL);
	plhs[13] = mxCreateDoubleMatrix(lengthSeries, 1, mxREAL);
	plhs[14] = mxCreateDoubleMatrix(lengthSeries, 1, mxREAL);
	plhs[15] = mxCreateDoubleMatrix(lengthSeries, 1, mxREAL);

	if (solver->Debug >= DETAILED)
	{
		plhs[16] = mxCreateDoubleMatrix(4, 1, mxREAL);
		double *plhseigHess = mxGetPr(plhs[16]);

		// Compute the smallest eigenvalue of the Hessian at initial iterate.
		SphereTx DomainPH0(Prob->GetDomain(), initialX);
		SphereTxRQ ProbHess0(Prob->GetDomain(), initialX, Prob, true);
		ProbHess0.SetDomain(&DomainPH0);
		Variable *TV00 = DomainPH0.RandominManifold();
		RTRNewton *RTRNewtonsolver = new RTRNewton(&ProbHess0, TV00);
		RTRNewtonsolver->Debug = NOOUTPUT;
		RTRNewtonsolver->Run();
		if (RTRNewtonsolver->Getnormgfgf0() > 1e-4)
			printf("Stop early when finding the smallest eigenvalue of the Hessian at initial iterate\n");
		plhseigHess[0] = RTRNewtonsolver->Getfinalfun(); // smallest eigenvalue 

		// Compute the largest eigenvalue of the Hessian at initial iterate.
		delete RTRNewtonsolver;
		ProbHess0.SetMinorMax(false);
		RTRNewtonsolver = new RTRNewton(&ProbHess0, TV00);
		RTRNewtonsolver->Debug = NOOUTPUT;
		RTRNewtonsolver->Run();
		if (RTRNewtonsolver->Getnormgfgf0() > 1e-4)
			printf("Stop early when finding the largest eigenvalue of the Hessian at initial iterate\n");
		plhseigHess[1] = -(RTRNewtonsolver->Getfinalfun()); // largest eigenvalue
		delete TV00;

		// Compute the smallest eigenvalue of the Hessian at root.
		Variable *root = initialX->ConstructEmpty();
		solver->GetXopt()->CopyTo(root);
		SphereTx DomainPH(Prob->GetDomain(), root);
		SphereTxRQ ProbHess(Prob->GetDomain(), root, Prob, true);
		ProbHess.SetDomain(&DomainPH);
		Variable *TV0 = DomainPH.RandominManifold();
		delete RTRNewtonsolver;
		RTRNewtonsolver = new RTRNewton(&ProbHess, TV0);
		RTRNewtonsolver->Debug = NOOUTPUT;
		RTRNewtonsolver->Run();
		if (RTRNewtonsolver->Getnormgfgf0() > 1e-4)
			printf("Stop early when finding the smallest eigenvalue of the Hessian at optimum\n");
		plhseigHess[2] = RTRNewtonsolver->Getfinalfun(); // smallest eigenvalue 

		// Compute the largest eigenvalue of the Hessian at root.
		delete RTRNewtonsolver;
		ProbHess.SetMinorMax(false);
		RTRNewtonsolver = new RTRNewton(&ProbHess, TV0);
		RTRNewtonsolver->Debug = NOOUTPUT;
		RTRNewtonsolver->Run();
		if (RTRNewtonsolver->Getnormgfgf0() > 1e-4)
			printf("Stop early when finding the largest eigenvalue of the Hessian at optimum\n");
		plhseigHess[3] = - (RTRNewtonsolver->Getfinalfun()); // largest eigenvalue
		delete RTRNewtonsolver;
		delete root;
		delete TV0;
	}

	double *plhsfun = mxGetPr(plhs[12]), *plhsgrad = mxGetPr(plhs[13]), *plhstime = mxGetPr(plhs[14]), *plhsdist = mxGetPr(plhs[15]);
	for (integer i = 0; i < lengthSeries; i++)
	{
		plhsfun[i] = solver->GetfunSeries()[i];
		plhsgrad[i] = solver->GetgradSeries()[i];
		plhstime[i] = solver->GettimeSeries()[i];
		plhsdist[i] = solver->GetdistSeries()[i];
	}

	tmp = mexProblem::GetFieldbyName(SolverParams, 0, "IsCheckGradHess");
	if (tmp != nullptr)
	{
		if (fabs(mxGetScalar(tmp)) > std::numeric_limits<double>::epsilon()) // if the value is nonzero
		{
			Prob->CheckGradHessian(initialX);
			// Check gradient and Hessian
			const Variable *xopt = solver->GetXopt();
			Variable *xoptcopy = xopt->ConstructEmpty();
			xopt->CopyTo(xoptcopy);
			xoptcopy->RemoveAllFromTempData();
			Prob->CheckGradHessian(xoptcopy);
            delete xoptcopy;
		}
	}

	delete solver;
};

bool ParseManiParams(const mxArray *ManiParams, Manifold **&manifolds, Element **&elements,
	integer *&powsinterval, integer &numoftype, integer &numoftotal)
{
	// Parse ManiParams
	integer nfields4 = mxGetNumberOfFields(ManiParams);
	numoftype = mxGetNumberOfElements(ManiParams);
	powsinterval = new integer[numoftype + 1];
	char name[30] = "";
	manifolds = new Manifold *[numoftype];
	integer n, p, m, Params;
	powsinterval[0] = 0;

	for (integer i = 0; i < numoftype; i++)
		powsinterval[i + 1] = powsinterval[i] + mxGetScalar(mexProblem::GetFieldbyName(ManiParams, i, "numofmani"));

	numoftotal = powsinterval[numoftype];
	elements = new Element *[numoftotal];
	PARAMSMAP params;
	for (integer i = 0; i < numoftype; i++)
	{
		if (mxGetString(mexProblem::GetFieldbyName(ManiParams, i, "name"), name, 30))
			mexErrMsgTxt("error in getting manifold name!");
		n = mxGetScalar(mexProblem::GetFieldbyName(ManiParams, i, "n"));
		m = mxGetScalar(mexProblem::GetFieldbyName(ManiParams, i, "m"));
		p = mxGetScalar(mexProblem::GetFieldbyName(ManiParams, i, "p"));
		Params = mxGetScalar(mexProblem::GetFieldbyName(ManiParams, i, "ParamSet"));
		params[static_cast<std::string> ("ParamSet")] = Params;
		//		params.insert(std::pair<std::string, double>("ParamSet", Params));

		manifolds[i] = GetAManifold(name, n, m, p);
		manifolds[i]->SetParams(params);

		for (integer j = powsinterval[i]; j < powsinterval[i + 1]; j++)
		{
			elements[j] = GetAnElement(name, n, m, p);
		}
		if (manifolds[i] == nullptr || elements[i] == nullptr)
		{
			return false;
		}
	}

	return true;
};

Manifold *GetAManifold(const char *name, integer n, integer m, integer p)
{
	if (strcmp(name, "Euclidean") == 0)
	{
		return new Euclidean(n, m);
	}
	else
	if (strcmp(name, "Sphere") == 0)
	{
		return new Sphere(n);
	}
	else
	if (strcmp(name, "Stiefel") == 0)
	{
		return new Stiefel(n, p);
	}
	else
	if (strcmp(name, "Oblique") == 0)
	{
		return new Oblique(n, m);
	}
	else
	if (strcmp(name, "LowRank") == 0)
	{
		return new LowRank(n, m, p);
	}
	else
	if (strcmp(name, "OrthGroup") == 0)
	{
		return new OrthGroup(n);
	}
	else
	if (strcmp(name, "L2Sphere") == 0)
	{
		return new L2Sphere(n);
	}
	else
	if (strcmp(name, "SPDManifold") == 0)
	{
		return new SPDManifold(n);
	}
	else
	if (strcmp(name, "CpxNStQOrth") == 0)
	{
		return new CpxNStQOrth(n, p);
	}
	else
	if (strcmp(name, "Grassmann") == 0)
	{
		return new Grassmann(n, p);
	}
	else
	if (strcmp(name, "EucPositive") == 0)
	{
		return new EucPositive(n, m);
	}
	else
	if (strcmp(name, "SPDTensor") == 0)
	{
		return new SPDTensor(n, m);
	}
	else
	{
		printf("Manifold: %s does not implemented in this library!\n", name);
		return nullptr;
	}
};

Element *GetAnElement(const char *name, integer n, integer m, integer p)
{
	if (strcmp(name, "Euclidean") == 0)
	{
		return new EucVariable(n, m);
	}
	else
	if (strcmp(name, "Sphere") == 0)
	{
		return new SphereVariable(n);
	}
	else
	if (strcmp(name, "Stiefel") == 0)
	{
		return new StieVariable(n, p);
	}
	else
	if (strcmp(name, "Oblique") == 0)
	{
		return new ObliqueVariable(n, m);
	}
	else
	if (strcmp(name, "LowRank") == 0)
	{
		return new LowRankVariable(n, m, p);
	}
	else
	if (strcmp(name, "OrthGroup") == 0)
	{
		return new OrthGroupVariable(n);
	}
	else
	if (strcmp(name, "L2Sphere") == 0)
	{
		return new L2SphereVariable(n);
	}
	else
	if (strcmp(name, "SPDManifold") == 0)
	{
		return new SPDVariable(n);
	}
	else
	if (strcmp(name, "CpxNStQOrth") == 0)
	{
		return new CSOVariable(n, p);
	}
	else
	if (strcmp(name, "Grassmann") == 0)
	{
		return new GrassVariable(n, p);
	}
	else
	if (strcmp(name, "EucPositive") == 0)
	{
		return new EucPosVariable(n, m);
	}
	else
	if (strcmp(name, "SPDTensor") == 0)
	{
		return new SPDTVariable(n, m);
	}
	else
	{
		printf("Element: %s does not implemented in this library!\n", name);
		return nullptr;
	}
};
#endif // end of MATLAB_MEX_FILE
#endif // end of DRIVERMEXPROB_H
