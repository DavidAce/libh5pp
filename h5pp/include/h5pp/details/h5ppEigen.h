#pragma once

#if __has_include(<Eigen/Core>)
    #define H5PP_EIGEN3
    #include <Eigen/Core>
    #include <unsupported/Eigen/CXX11/Tensor>
#endif

#include <iostream>
#include <iterator>
#include <numeric>
#if !defined(_MSC_VER)
    #define H5PP_CONSTEXPR constexpr
#else
    #define H5PP_CONSTEXPR
#endif

namespace h5pp {
/*! \brief **Textra** stands for "Tensor Extra". Provides extra functionality to Eigen::Tensor.*/

/*!
 *  \namespace eigen
 *  This namespace makes shorthand typedef's to Eigen's unsupported Tensor module, and provides handy functions
 *  to interface between `Eigen::Tensor` and `Eigen::Matrix` objects.
 *  The contents of this namespace is co clear it is self-documenting ;)
 */
#ifdef H5PP_EIGEN3
    namespace eigen {
        // The default type for an index varies wildly between compilers/archs, so we query here
//        using Eigen::Index = typename Eigen::Index;
        template<typename Scalar>
        using MatrixType = Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic>;
        template<typename Scalar>
        using VectorType = Eigen::Matrix<Scalar, Eigen::Dynamic, 1>;
        template<Eigen::Index rank>
        using array = Eigen::array<Eigen::Index, rank>;
        template<Eigen::Index rank>
        using DSizes = Eigen::DSizes<Eigen::Index, rank>;
        using array8 = array<8>;
        using array7 = array<7>;
        using array6 = array<6>;
        using array5 = array<5>;
        using array4 = array<4>;
        using array3 = array<3>;
        using array2 = array<2>;
        using array1 = array<1>;

        // Shorthand for the list of index pairs.
        template<typename T, auto length>
        using idxlistpair = Eigen::array<Eigen::IndexPair<T>, length>;

        // Handy functions to copy lists of dimensions
        template<typename T, auto rank, typename Container>
        void copy_dims(Eigen::DSizes<T, rank> dsizes, const Container &container) {
            if constexpr(std::is_array_v<Container>) {
                std::copy_n(std::begin(container), rank, dsizes.begin());
            } else {
                if(container.size() != rank) throw std::runtime_error("copy_dims: Wrong container size, can't copy dimensions.");
                std::copy(std::begin(container), std::end(container), dsizes.begin());
            }
        }
        template<auto rank, typename Container>
        [[nodiscard]] H5PP_CONSTEXPR Eigen::DSizes<Eigen::Index, rank> copy_dims(const Container &container) {
            Eigen::DSizes<Eigen::Index, rank> dsizes;
            if constexpr(std::is_array_v<Container>) {
                std::copy_n(std::begin(container), rank, dsizes.begin());
            } else {
                if(container.size() != rank) throw std::runtime_error("copy_dims: Wrong container size, can't copy dimensions.");
                std::copy(std::begin(container), std::end(container), dsizes.begin());
            }
            return dsizes;
        }

        template<auto rank, typename Container>
        [[nodiscard]] H5PP_CONSTEXPR Eigen::DSizes<Eigen::Index, rank> copy_dims(const Container *container) {
            Eigen::DSizes<Eigen::Index, rank> dsizes;
            std::copy_n(container, rank, dsizes.begin());
            return dsizes;
        }

        H5PP_CONSTEXPR idxlistpair<Eigen::Index, 0> idx() {
            Eigen::array<Eigen::IndexPair<Eigen::Index>, 0> empty_index_list = {};
            return empty_index_list;
        }

        template<typename T, size_t N>
        H5PP_CONSTEXPR idxlistpair<Eigen::Index, N> idx(const T (&list1)[N], const T (&list2)[N]) {
            // Use numpy-style indexing for contraction. Each list contains a list of indices to be contracted for the respective
            // tensors. This function zips them together into pairs as used in Eigen::Tensor module. This does not sort the indices in decreasing order.
            static_assert(std::is_integral_v<T>);
            Eigen::array<Eigen::IndexPair<Eigen::Index>, N> pairlistOut;
            for(size_t i = 0; i < N; i++) { pairlistOut[i] = Eigen::IndexPair<Eigen::Index>{list1[i], list2[i]}; }
            return pairlistOut;
        }

        template<typename T>
        struct idx_dim_pair {
            T idxA;
            T idxB;
            T dimB;
        };

        template<size_t NB, size_t N>
        H5PP_CONSTEXPR idxlistpair<Eigen::Index, N> sortIdx(const array<NB> &dimensions, const Eigen::Index (&idx_ctrct_A)[N], const Eigen::Index (&idx_ctrct_B)[N]) {
            // When doing contractions, some indices may be larger than others. For performance, you want to
            // contract the largest indices first. This will return a sorted index list in decreasing order.
            Eigen::array<idx_dim_pair<Eigen::Index>, N> idx_dim_pair_list;
            for(size_t i = 0; i < N; i++) { idx_dim_pair_list[i] = {idx_ctrct_A[i], idx_ctrct_B[i], dimensions[idx_ctrct_B[i]]}; }
            std::sort(idx_dim_pair_list.begin(), idx_dim_pair_list.end(), [](const auto &i, const auto &j) { return i.dimB > j.dimB; });
            idxlistpair<Eigen::Index, N> pairlistOut;
            for(size_t i = 0; i < N; i++) { pairlistOut[i] = Eigen::IndexPair<Eigen::Index>{idx_dim_pair_list[i].idxA, idx_dim_pair_list[i].idxB}; }
            return pairlistOut;
        }

        //
        //    //***************************************//
        //    //Different views for rank 1 and 2 tensors//
        //    //***************************************//
        //

        template<typename Scalar>
        constexpr Eigen::Tensor<Scalar, 1> extractDiagonal(const Eigen::Tensor<Scalar, 2> &tensor) {
            auto rows = tensor.dimension(0);
            auto cols = tensor.dimension(1);
            assert(tensor.dimension(0) == tensor.dimension(1) and "extractDiagonal expects a square tensor");

            Eigen::Tensor<Scalar, 1> diagonals(rows);
            for(auto i = 0; i < rows; i++) { diagonals(i) = tensor(i, i); }
            return diagonals;
        }

        template<typename Scalar>
        constexpr auto asDiagonal(const Eigen::Tensor<Scalar, 1> &tensor) {
            return tensor.inflate(array1{tensor.size() + 1}).reshape(array2{tensor.size(), tensor.size()});
        }

        template<typename Scalar>
        constexpr auto asDiagonalSquared(const Eigen::Tensor<Scalar, 1> &tensor) {
            return tensor.square().inflate(array1{tensor.size() + 1}).reshape(array2{tensor.size(), tensor.size()});
        }

        template<typename Scalar>
        constexpr auto asDiagonalInversed(const Eigen::Tensor<Scalar, 1> &tensor) {
            return tensor.inverse().inflate(array1{tensor.size() + 1}).reshape(array2{tensor.size(), tensor.size()});
        }

        template<typename Scalar>
        constexpr auto asDiagonalInversed(const Eigen::Tensor<Scalar, 2> &tensor) {
            assert(tensor.dimension(0) == tensor.dimension(1) and "Textra::asDiagonalInversed expects a square tensor");
            Eigen::Tensor<Scalar, 2> inversed = asDiagonalInversed(extractDiagonal(tensor));
            std::cout << "inversed:\n" << inversed << std::endl;
            return inversed;
        }

        template<typename Scalar>
        constexpr auto asNormalized(const Eigen::Tensor<Scalar, 1> &tensor) {
            Eigen::Map<const VectorType<Scalar>> map(tensor.data(), tensor.size());
            return Eigen::TensorMap<Eigen::Tensor<const Scalar, 1>>(map.normalized().eval().data(), array1{map.size()});
        }

        //    //****************************//
        //    //Matrix to tensor conversions//
        //    //****************************//

        // Detects if Derived is a plain object, like "MatrixXd" or similar.
        // std::decay removes pointer or ref qualifiers if present
        template<typename Derived>
        using is_plainObject = std::is_base_of<Eigen::PlainObjectBase<std::decay_t<Derived>>, std::decay_t<Derived>>;
        template<typename Derived>
        using is_matrixObject = std::is_base_of<Eigen::MatrixBase<std::decay_t<Derived>>, std::decay_t<Derived>>;
        template<typename Derived>
        using is_arrayObject = std::is_base_of<Eigen::ArrayBase<std::decay_t<Derived>>, std::decay_t<Derived>>;

        template<typename Derived, auto rank>
        constexpr Eigen::Tensor<typename Derived::Scalar, rank> Matrix_to_Tensor(const Eigen::EigenBase<Derived> &matrix, const array<rank> &dims) {
            if constexpr(is_plainObject<Derived>::value) {
                // Return map from raw input.
                return Eigen::TensorMap<const Eigen::Tensor<const typename Derived::Scalar, rank>>(matrix.derived().eval().data(), dims);
            } else {
                // Create a temporary
                MatrixType<typename Derived::Scalar> matref = matrix;
                return Eigen::TensorMap<Eigen::Tensor<typename Derived::Scalar, rank>>(matref.data(), dims);
            }
        }

        // Helpful overload
        template<typename Derived, typename... Dims>
        constexpr Eigen::Tensor<typename Derived::Scalar, sizeof...(Dims)> Matrix_to_Tensor(const Eigen::EigenBase<Derived> &matrix, const Dims... dims) {
            return Matrix_to_Tensor(matrix, array<sizeof...(Dims)>{dims...});
        }
        // Helpful overload
        template<typename Derived, auto rank>
        constexpr Eigen::Tensor<typename Derived::Scalar, rank> Matrix_to_Tensor(const Eigen::EigenBase<Derived> &matrix, const DSizes<rank> &dims) {
            array<rank> dim_array = dims;
            std::copy(std::begin(dims), std::end(dims), std::begin(dim_array));
            return Matrix_to_Tensor(matrix, dim_array);
        }

        template<typename Derived>
        constexpr auto Matrix_to_Tensor1(const Eigen::EigenBase<Derived> &matrix) {
            return Matrix_to_Tensor(matrix, matrix.size());
        }
        template<typename Derived>
        constexpr auto Matrix_to_Tensor2(const Eigen::EigenBase<Derived> &matrix) {
            return Matrix_to_Tensor(matrix, matrix.rows(), matrix.cols());
        }

        //****************************//
        // Tensor to matrix conversions//
        //****************************//

        template<typename Scalar>
        constexpr MatrixType<Scalar> Tensor2_to_Matrix(const Eigen::Tensor<Scalar, 2> &tensor) {
            return Eigen::Map<const MatrixType<Scalar>>(tensor.data(), tensor.dimension(0), tensor.dimension(1));
        }

        template<typename Scalar>
        constexpr MatrixType<Scalar> Tensor1_to_Vector(const Eigen::Tensor<Scalar, 1> &tensor) {
            return Eigen::Map<const VectorType<Scalar>>(tensor.data(), tensor.size());
        }

        template<typename Scalar, auto rank, typename sizeType>
        constexpr MatrixType<Scalar> Tensor_to_Matrix(const Eigen::Tensor<Scalar, rank> &tensor, const sizeType rows, const sizeType cols) {
            return Eigen::Map<const MatrixType<Scalar>>(tensor.data(), rows, cols);
        }

        //************************//
        // change storage layout //
        //************************//
        template<typename Derived>
        auto to_RowMajor(const Eigen::TensorBase<Derived, Eigen::ReadOnlyAccessors> &tensor) {
            if constexpr(Eigen::RowMajor == static_cast<Eigen::StorageOptions>(Derived::Layout))
                return tensor;
            else {
                array<Derived::NumIndices> neworder;
                std::iota(std::begin(neworder), std::end(neworder), 0);
                std::reverse(neworder.data(), neworder.data() + neworder.size());
                return Eigen::Tensor<typename Derived::Scalar, Derived::NumIndices, Eigen::RowMajor>(tensor.swap_layout().shuffle(neworder));
            }
        }

        template<typename Derived>
        auto to_ColMajor(const Eigen::TensorBase<Derived, Eigen::ReadOnlyAccessors> &tensor) {
            if constexpr(Eigen::ColMajor == static_cast<Eigen::StorageOptions>(Derived::Layout))
                return tensor;
            else {
                array<Derived::NumIndices> neworder;
                std::iota(std::begin(neworder), std::end(neworder), 0);
                std::reverse(neworder.data(), neworder.data() + neworder.size());
                return Eigen::Tensor<typename Derived::Scalar, Derived::NumIndices, Eigen::ColMajor>(tensor.swap_layout().shuffle(neworder));
            }
        }

        template<typename Derived>
        auto to_RowMajor(const Eigen::DenseBase<Derived> &dense) {
            if constexpr(Derived::IsRowMajor) { return dense; }
            if constexpr(is_matrixObject<Derived>::value) {
                return Eigen::Matrix<typename Derived::Scalar, Derived::RowsAtCompileTime, Derived::ColsAtCompileTime, Eigen::RowMajor>(dense);
            } else if constexpr(is_arrayObject<Derived>::value) {
                return Eigen::Array<typename Derived::Scalar, Derived::RowsAtCompileTime, Derived::ColsAtCompileTime, Eigen::RowMajor>(dense);
            }
            throw std::runtime_error("Wrong dense type?? Report this bug!");
        }

        template<typename Derived>
        auto to_ColMajor(const Eigen::DenseBase<Derived> &dense) {
            if constexpr(not Derived::IsRowMajor) { return dense; }
            if constexpr(is_matrixObject<Derived>::value) {
                return Eigen::Matrix<typename Derived::Scalar, Derived::RowsAtCompileTime, Derived::ColsAtCompileTime, Eigen::ColMajor>(dense);
            } else if constexpr(is_arrayObject<Derived>::value) {
                return Eigen::Array<typename Derived::Scalar, Derived::RowsAtCompileTime, Derived::ColsAtCompileTime, Eigen::ColMajor>(dense);
            }
            throw std::runtime_error("Wrong dense type?? Report this bug!");
        }
    }

#endif

}
