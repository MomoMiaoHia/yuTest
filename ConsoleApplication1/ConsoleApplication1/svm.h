#ifndef _LIBSVM_H
#define _LIBSVM_H

#define LIBSVM_VERSION 321

#ifdef __cplusplus
extern "C" {
#endif

	extern int libsvm_version;

	struct svm_node        //SVM节点，即：索引i及其所对应的第i个特征值
	{
		int index;
		double value;
	};

	struct svm_problem      //此结构体即是训练集，以及测试集
	{
		int l;          //训练样本的个数
		double *y;        //l个训练样本中每个训练样本的类别号，即标签
		struct svm_node **x;    //指向指针的指针，可看成一个矩阵，该矩阵指向的内容是所有训练样本的所有的特征值数据
	};

	enum { C_SVC, NU_SVC, ONE_CLASS, EPSILON_SVR, NU_SVR };	/* svm_type */
	enum { LINEAR, POLY, RBF, SIGMOID, PRECOMPUTED }; /* kernel_type */

	struct svm_parameter
	{
		int svm_type;     //svm的类型，比如分类，回归
		int kernel_type;    //核函数的类型，比如线性核，RBF核
		int degree;	/* for poly */
		double gamma;	/* for poly/rbf/sigmoid */
		double coef0;	/* for poly/sigmoid */

						/* these are for training only */
		double cache_size; /* in MB */                 //核缓存大小，以MB为单位
		double eps;	/* stopping criteria */                 //误差精度小于eps时，停止训练
		double C;	/* for C_SVC, EPSILON_SVR and NU_SVR */         //惩罚因子
		int nr_weight;		// for C_SVC  ,nr_weight是weight_label和weight的元素个数，或者称之为类别数
		int *weight_label;	//Weight[i]与weight_label[i]之间是一一对应的，
		double* weight;		//weight[i]代表着类别weight_label[i]的惩罚因子的系数是weight[i]。
		double nu;	/* for NU_SVC, ONE_CLASS, and NU_SVR */
		double p;	/* for EPSILON_SVR */
		int shrinking;	/* use the shrinking heuristics */      //等于1代表执行启发式收缩
		int probability; /* do probability estimates */        //等于1代表模型的分布概率已知
	};

	//
	// svm_model
	// 
	struct svm_model        //此结构体为训练生成的model，用于数据的测试
	{
		struct svm_parameter param;	/* parameter */
		int nr_class;		/* number of classes, = 2 in regression/one class svm */
		int l;			/* total #SV */          //支持向量个数
		struct svm_node **SV;		/* SVs (SV[l]) */           //支持向量
		double **sv_coef;	/* coefficients for SVs in decision functions (sv_coef[k-1][l]) */
		double *rho;		/* constants in decision functions (rho[k*(k-1)/2]) */
		double *probA;		/* pariwise probability information */
		double *probB;
		int *sv_indices;        /* sv_indices[0,...,nSV-1] are values in [1,...,num_traning_data] to indicate SVs in the training set */

								/* for classification only */

		int *label;		/* label of each class (label[k]) */
		int *nSV;		/* number of SVs for each class (nSV[k]) */
						/* nSV[0] + nSV[1] + ... + nSV[k-1] = l */
						/* XXX */
		int free_sv;		//如果svm_model已经通过svm_load_model创建，则该值为1；如果svm_model是通过svm_train创建的，该值为0
	};

	struct svm_model *svm_train(const struct svm_problem *prob, const struct svm_parameter *param);  //对于训练数据和svm的设置参数，返回一个model，用于测试
	void svm_cross_validation(const struct svm_problem *prob, const struct svm_parameter *param, int nr_fold, double *target);

	int svm_save_model(const char *model_file_name, const struct svm_model *model);        //保存训练好的模型
	struct svm_model *svm_load_model(const char *model_file_name);

	int svm_get_svm_type(const struct svm_model *model);
	int svm_get_nr_class(const struct svm_model *model);
	void svm_get_labels(const struct svm_model *model, int *label);
	void svm_get_sv_indices(const struct svm_model *model, int *sv_indices);
	int svm_get_nr_sv(const struct svm_model *model);
	double svm_get_svr_probability(const struct svm_model *model);

	double svm_predict_values(const struct svm_model *model, const struct svm_node *x, double* dec_values);
	double svm_predict(const struct svm_model *model, const struct svm_node *x);      //预测函数
	double svm_predict_probability(const struct svm_model *model, const struct svm_node *x, double* prob_estimates);

	void svm_free_model_content(struct svm_model *model_ptr);
	void svm_free_and_destroy_model(struct svm_model **model_ptr_ptr);
	void svm_destroy_param(struct svm_parameter *param);

	const char *svm_check_parameter(const struct svm_problem *prob, const struct svm_parameter *param);//为了防止错误的参数设置，可以调用该函数来对输入参数进行检查。
	int svm_check_probability_model(const struct svm_model *model);

	void svm_set_print_string_function(void(*print_func)(const char *));

#ifdef __cplusplus
}
#endif

#endif /* _LIBSVM_H */
