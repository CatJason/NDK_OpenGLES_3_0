package com.byteflow.app.adapter

import android.content.Context
import android.graphics.Color
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.RadioButton
import android.widget.TextView
import androidx.recyclerview.widget.RecyclerView
import com.byteflow.app.R
import com.byteflow.app.adapter.MyRecyclerViewAdapter.MyViewHolder

/**
 * RecyclerView 的适配器类，用于展示一组带有单选按钮的列表项。
 * @param mContext 上下文对象，用于访问资源。
 * @param mTitles 列表项标题数据。
 */
class MyRecyclerViewAdapter(private val mContext: Context, private val mTitles: List<String>) :
    RecyclerView.Adapter<MyViewHolder>(), View.OnClickListener {

    // 当前选中的列表项索引
    var selectIndex = 0

    // 列表项点击监听器
    private var mOnItemClickListener: OnItemClickListener? = null

    /**
     * 添加列表项点击监听器。
     * @param onItemClickListener 点击监听器实例。
     */
    fun addOnItemClickListener(onItemClickListener: OnItemClickListener?) {
        mOnItemClickListener = onItemClickListener
    }

    /**
     * 创建 ViewHolder，当 RecyclerView 需要新视图时调用。
     * @param parent 父级视图组。
     * @param viewType 视图类型（未使用）。
     * @return MyViewHolder 实例。
     */
    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): MyViewHolder {
        val view =
            LayoutInflater.from(parent.context).inflate(R.layout.sample_item_layout, parent, false)
        val myViewHolder = MyViewHolder(view)
        view.setOnClickListener(this) // 设置点击事件
        return myViewHolder
    }

    /**
     * 绑定数据到 ViewHolder，当 RecyclerView 需要更新列表项视图时调用。
     * @param holder 当前列表项的 ViewHolder。
     * @param position 当前列表项的索引。
     */
    override fun onBindViewHolder(holder: MyViewHolder, position: Int) {
        with(holder) {
            mTitle.text = mTitles[position] // 设置标题文本
            if (position == selectIndex) { // 如果当前索引是选中项
                mRadioButton.isChecked = true
                mTitle.setTextColor(mContext.resources.getColor(R.color.colorAccent)) // 设置高亮颜色
            } else { // 如果不是选中项
                mRadioButton.isChecked = false
                mTitle.text = mTitles[position]
                mTitle.setTextColor(Color.GRAY) // 设置为灰色
            }
            itemView.tag = position // 为视图设置索引标签
        }
    }

    /**
     * 返回列表项数量。
     * @return 列表项总数。
     */
    override fun getItemCount(): Int {
        return mTitles.size
    }

    /**
     * 列表项点击事件回调方法。
     * @param v 被点击的视图。
     */
    override fun onClick(v: View) {
        mOnItemClickListener?.onItemClick(v, v.tag as Int)
    }

    /**
     * 列表项点击监听器接口。
     */
    interface OnItemClickListener {
        /**
         * 当列表项被点击时调用。
         * @param view 被点击的视图。
         * @param position 被点击视图的索引。
         */
        fun onItemClick(view: View?, position: Int)
    }

    /**
     * 自定义 ViewHolder 类，用于缓存视图控件。
     * @param itemView 列表项视图。
     */
    inner class MyViewHolder(itemView: View) : RecyclerView.ViewHolder(itemView) {
        var mRadioButton: RadioButton // 单选按钮
        var mTitle: TextView // 标题文本

        init {
            mRadioButton = itemView.findViewById(R.id.radio_btn) // 获取单选按钮视图
            mTitle = itemView.findViewById(R.id.item_title) // 获取标题文本视图
        }
    }
}