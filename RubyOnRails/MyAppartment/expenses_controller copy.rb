class ExpensesController < ApplicationController
  before_action :authenticate_user!
  before_action :set_expense, only: [:show, :edit, :update, :destroy]
  before_action :add_resources, only: [:index, :show, :new, :show_by_month, :create]
  before_action :require_permission, only: [:new, :create, :update, :edit, :destroy, :show]
  before_action only: [:index, :show_by_month] {allow_users(:manager, :owner)}  

  # GET /expenses
  # GET /expenses.json
  require 'spreadsheet'
  require 'sbif'

  Spreadsheet.client_encoding = 'UTF-8'

  def upload
  end

  def index
    # sbif = SBIF.new(:api_key => "4a9ae41dffc8d3b5a972cf8a25c41e0515be5b1f")
    #response = HTTParty.get('http://api.sbif.cl/api-sbifv3/recursos_api/dolar?apikey=4a9ae41dffc8d3b5a972cf8a25c41e0515be5b1f&formato=json')
    #@dolar = response['Dolares'][0]['Valor']
    @months = Expense.uniq.pluck(:month)
  end

  def show_by_month
    response = HTTParty.get('http://api.sbif.cl/api-sbifv3/recursos_api/dolar?apikey=4a9ae41dffc8d3b5a972cf8a25c41e0515be5b1f&formato=json')
    @dolar = response['Dolares'][0]['Valor']
    @months = Expense.uniq.pluck(:month)
    month = DateTime.parse(params[:month])
    @expenses = Expense.all.select {|exp| exp.month == month}
  end

  # GET /expenses/1
  # GET /expenses/1.json
  def show
  end

  # GET /expenses/new
  def new
    @expense = Expense.new
  end

  # GET /expenses/1/edit
  def edit
  end

  # POST /expenses
  # POST /expenses.json
  def create

    path = params[:expense][:path_planilla]
    object_path = Rails.root.join(path)
    workbook = Spreadsheet.open object_path
    sheet1 = workbook.worksheet 0
    sheet1.each do |row|
      next if row[0] == "Apt"

      break if row[0].nil? # if first cell empty
      @expense = Expense.new
      @expense.apartment_id = row[0]
      @expense.common_expense = row[1]
      @expense.particular_expense = row[2]
      @expense.debt = row[3]
      @expense.month = row[4]

      if @expense.valid?
        @expense.save
      end
    end

    #@expense = Expense.new(expense_params)

    respond_to do |format|
      if @expense.save
        format.html { redirect_to expenses_path, notice: 'Expense was successfully created.' }
        format.json { render :show, status: :created, location: @expense }
      else
        format.html { render :new }
        format.json { render json: @expense.errors, status: :unprocessable_entity }
      end
    end
  end

  # PATCH/PUT /expenses/1
  # PATCH/PUT /expenses/1.json
  def update
    respond_to do |format|
      if @expense.update(expense_params)
        format.html { redirect_to @expense, notice: 'Expense was successfully updated.' }
        format.json { render :show, status: :ok, location: @expense }
      else
        format.html { render :edit }
        format.json { render json: @expense.errors, status: :unprocessable_entity }
      end
    end
  end

  # DELETE /expenses/1
  # DELETE /expenses/1.json
  def destroy
    @expense.destroy
    respond_to do |format|
      format.html { redirect_to expenses_url, notice: 'Expense was successfully destroyed.' }
      format.json { head :no_content }
    end
  end

  private
    # Use callbacks to share common setup or constraints between actions.
    def set_expense
      @expense = Expense.find(params[:id])
    end

    # Never trust parameters from the scary internet, only allow the white list through.
    def expense_params
      params.require(:expense).permit(:common_expense, :particular_expense, :debt, :month)
    end
    def add_resources
      @icons = {"building" => "check-circle", "expenses" => "usd",
      "discussion" => "question-circle", "complaints" => "exclamation"}
      @translated = {"building" => "Espacios Comunes", "expenses" => "Gastos Comunes", "discussion" => "Discusión General",
          "complaints" => "Reclamos"}
    end
end