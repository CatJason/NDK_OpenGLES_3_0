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

class MyRecyclerViewAdapter(private val mContext: Context, private val mTitles: List<String>) :
    RecyclerView.Adapter<MyViewHolder>(), View.OnClickListener {
    var selectIndex = 0
    private var mOnItemClickListener: OnItemClickListener? = null
    fun addOnItemClickListener(onItemClickListener: OnItemClickListener?) {
        mOnItemClickListener = onItemClickListener
    }

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): MyViewHolder {
        val view =
            LayoutInflater.from(parent.context).inflate(R.layout.sample_item_layout, parent, false)
        val myViewHolder = MyViewHolder(view)
        view.setOnClickListener(this)
        return myViewHolder
    }

    override fun onBindViewHolder(holder: MyViewHolder, position: Int) {
        with(holder) {
            mTitle.text = mTitles[position]
            if (position == selectIndex) {
                mRadioButton.isChecked = true
                mTitle.setTextColor(mContext.resources.getColor(R.color.colorAccent))
            } else {
                mRadioButton.isChecked = false
                mTitle.text = mTitles[position]
                mTitle.setTextColor(Color.GRAY)
            }
            itemView.tag = position
        }
    }

    override fun getItemCount(): Int {
        return mTitles.size
    }

    override fun onClick(v: View) {
        mOnItemClickListener?.onItemClick(v, v.tag as Int)
    }

    interface OnItemClickListener {
        fun onItemClick(view: View?, position: Int)
    }

    inner class MyViewHolder(itemView: View) : RecyclerView.ViewHolder(itemView) {
        var mRadioButton: RadioButton
        var mTitle: TextView

        init {
            mRadioButton = itemView.findViewById(R.id.radio_btn)
            mTitle = itemView.findViewById(R.id.item_title)
        }
    }
}